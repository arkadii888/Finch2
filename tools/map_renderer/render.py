"""Render an offline OSM and GeoTIFF context map for multimodal inference."""

from __future__ import annotations

import io
import math
import os
import tempfile
from pathlib import Path

os.environ.setdefault("MPLBACKEND", "Agg")
os.environ.setdefault(
    "MPLCONFIGDIR",
    str(Path(tempfile.gettempdir()) / "finch2-matplotlib"),
)

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
from PIL import Image

TILE_SIZE = 256


def meters_to_degrees(latitude_deg: float, distance_m: float) -> tuple[float, float]:
    """Convert a meter distance to lat/lon deltas at ``latitude_deg``."""
    latitude_delta = distance_m / 111_320.0
    longitude_delta = distance_m / (111_320.0 * max(math.cos(math.radians(latitude_deg)), 1e-6))
    return latitude_delta, longitude_delta


def bbox_from_center(
    latitude_deg: float,
    longitude_deg: float,
    half_window_m: float,
) -> tuple[float, float, float, float]:
    """Return WGS84 bounds ``(west, south, east, north)`` around a point."""
    if not (-90.0 <= latitude_deg <= 90.0 and -180.0 <= longitude_deg <= 180.0):
        raise ValueError(f"Invalid WGS84 point: {latitude_deg}, {longitude_deg}")
    if not math.isfinite(half_window_m) or half_window_m <= 0.0:
        raise ValueError(f"Half-window must be positive: {half_window_m}")

    dlat, dlon = meters_to_degrees(latitude_deg, half_window_m)
    return (
        longitude_deg - dlon,
        latitude_deg - dlat,
        longitude_deg + dlon,
        latitude_deg + dlat,
    )


def latlon_to_tile(latitude_deg: float, longitude_deg: float, zoom: int) -> tuple[int, int]:
    """Convert WGS84 coordinates to a slippy-map tile index."""
    latitude_deg = max(min(latitude_deg, 85.051_128_78), -85.051_128_78)
    tile_count = 2**zoom
    x = int((longitude_deg + 180.0) / 360.0 * tile_count)
    latitude_rad = math.radians(latitude_deg)
    y = int((1.0 - math.asinh(math.tan(latitude_rad)) / math.pi) / 2.0 * tile_count)
    return x, y


def tile_bounds(x: int, y: int, zoom: int) -> tuple[float, float, float, float]:
    """Return a tile's WGS84 bounds as west, south, east, north."""
    tile_count = float(2**zoom)
    west = x / tile_count * 360.0 - 180.0
    east = (x + 1) / tile_count * 360.0 - 180.0
    north = math.degrees(math.atan(math.sinh(math.pi * (1.0 - 2.0 * y / tile_count))))
    south = math.degrees(math.atan(math.sinh(math.pi * (1.0 - 2.0 * (y + 1) / tile_count))))
    return west, south, east, north


def stitch_basemap(
    bounds: tuple[float, float, float, float],
    zoom: int,
    cache_dir: Path,
) -> np.ndarray:
    """Stitch cached OSM tiles and crop to exact WGS84 bounds."""
    if not (1 <= zoom <= 18):
        raise ValueError(f"Zoom must be between 1 and 18: {zoom}")

    west, south, east, north = bounds
    x_min, y_max = latlon_to_tile(south, west, zoom)
    x_max, y_min = latlon_to_tile(north, east, zoom)
    tile_count = 2**zoom
    x0, x1 = max(0, min(x_min, x_max)), min(tile_count, max(x_min, x_max) + 1)
    y0, y1 = max(0, min(y_min, y_max)), min(tile_count, max(y_min, y_max) + 1)
    if x0 >= x1 or y0 >= y1:
        raise ValueError(f"No tiles cover bounds {bounds} at zoom {zoom}")

    mosaic = Image.new("RGB", ((x1 - x0) * TILE_SIZE, (y1 - y0) * TILE_SIZE))
    for x in range(x0, x1):
        for y in range(y0, y1):
            tile_path = cache_dir / str(zoom) / str(x) / f"{y}.png"
            if not tile_path.is_file():
                raise FileNotFoundError(f"Missing cached tile z={zoom} x={x} y={y}: {tile_path}")
            with Image.open(tile_path) as source:
                tile = source.convert("RGB")
            if tile.size != (TILE_SIZE, TILE_SIZE):
                raise ValueError(f"Unexpected tile size {tile.size}: {tile_path}")
            mosaic.paste(tile, ((x - x0) * TILE_SIZE, (y - y0) * TILE_SIZE))

    mosaic_west, mosaic_south, _, _ = tile_bounds(x0, y1 - 1, zoom)
    _, _, mosaic_east, mosaic_north = tile_bounds(x1 - 1, y0, zoom)
    width, height = mosaic.size
    left = int(max(0, math.floor((west - mosaic_west) / (mosaic_east - mosaic_west) * width)))
    right = int(min(width, math.ceil((east - mosaic_west) / (mosaic_east - mosaic_west) * width)))
    top = int(max(0, math.floor((mosaic_north - north) / (mosaic_north - mosaic_south) * height)))
    bottom = int(
        min(height, math.ceil((mosaic_north - south) / (mosaic_north - mosaic_south) * height))
    )
    cropped = np.asarray(mosaic)[top:bottom, left:right]
    if cropped.size == 0:
        raise ValueError(f"Tile crop is empty for bounds {bounds}")
    return cropped


def sample_dem(
    dem_path: Path,
    bounds: tuple[float, float, float, float],
    output_shape: tuple[int, int],
) -> np.ndarray:
    """Reproject a DEM onto the rendered WGS84 map grid."""
    import rasterio
    from rasterio.transform import from_bounds
    from rasterio.warp import Resampling, reproject

    west, south, east, north = bounds
    height, width = output_shape
    destination = np.full((height, width), np.nan, dtype=np.float32)
    destination_transform = from_bounds(west, south, east, north, width, height)

    with rasterio.Env(GDAL_NUM_THREADS="1"), rasterio.open(dem_path) as source:
        if source.crs is None:
            raise ValueError(f"DEM has no CRS: {dem_path}")
        reproject(
            source=rasterio.band(source, 1),
            destination=destination,
            src_transform=source.transform,
            src_crs=source.crs,
            src_nodata=source.nodata,
            dst_transform=destination_transform,
            dst_crs="EPSG:4326",
            dst_nodata=np.nan,
            resampling=Resampling.bilinear,
        )
    return destination


def elevation_at(dem_path: Path, latitude_deg: float, longitude_deg: float) -> float:
    """Read DEM elevation in meters ASL at a WGS84 point."""
    import rasterio
    from rasterio.warp import transform

    with rasterio.Env(GDAL_NUM_THREADS="1"), rasterio.open(dem_path) as source:
        if source.crs is None:
            raise ValueError(f"DEM has no CRS: {dem_path}")
        xs, ys = [longitude_deg], [latitude_deg]
        if source.crs.to_string() != "EPSG:4326":
            xs, ys = transform("EPSG:4326", source.crs, xs, ys)
        x, y = xs[0], ys[0]
        if not (
            source.bounds.left <= x <= source.bounds.right
            and source.bounds.bottom <= y <= source.bounds.top
        ):
            raise ValueError(
                f"Point is outside DEM coverage: lat={latitude_deg}, lon={longitude_deg}"
            )
        value = float(next(source.sample([(x, y)]))[0])
        if not math.isfinite(value) or (source.nodata is not None and value == source.nodata):
            raise ValueError(f"No DEM data at lat={latitude_deg}, lon={longitude_deg}")
        return value


def contour_levels(values: np.ndarray, target_count: int = 8) -> np.ndarray:
    """Choose readable round contour levels for finite elevations."""
    minimum, maximum = float(np.min(values)), float(np.max(values))
    span = max(maximum - minimum, 1.0)
    step = 1.0
    for candidate in (1, 2, 5, 10, 20, 25, 50, 100, 200, 250, 500, 1_000):
        if span / candidate <= target_count + 2:
            step = float(candidate)
            break
    start = math.floor(minimum / step) * step
    stop = math.ceil(maximum / step) * step
    return np.arange(start, stop + step * 0.5, step)


def draw_scale_bar(
    axis,
    latitude_deg: float,
    bounds: tuple[float, float, float, float],
    half_window_m: float,
) -> None:
    """Draw a metric scale bar in the lower-left corner."""
    from matplotlib import patheffects

    target_m = max(half_window_m / 4.0, 50.0)
    choices = (50, 100, 200, 250, 500, 1_000, 2_000, 5_000)
    length_m = float(next((value for value in choices if value >= target_m), target_m))
    _, dlon = meters_to_degrees(latitude_deg, length_m)
    west, south, east, north = bounds
    width, height = east - west, north - south
    x0, y = west + 0.06 * width, south + 0.06 * height
    x1 = x0 + dlon
    axis.plot([x0, x1], [y, y], color="black", linewidth=3.0, zorder=8)
    axis.plot([x0, x1], [y, y], color="white", linewidth=1.2, zorder=9)
    label = axis.text(
        (x0 + x1) / 2.0,
        y + 0.02 * height,
        f"{length_m:.0f} m",
        horizontalalignment="center",
        fontweight="bold",
        zorder=10,
    )
    label.set_path_effects([patheffects.withStroke(linewidth=3.0, foreground="white")])


def render_snapshot(
    latitude_deg: float,
    longitude_deg: float,
    half_window_m: float,
    dem_path: Path,
    cache_dir: Path,
    zoom: int = 15,
) -> Image.Image:
    """Render the map image consumed by the vision-language model."""
    if not dem_path.is_file():
        raise FileNotFoundError(f"DEM not found: {dem_path}")

    bounds = bbox_from_center(latitude_deg, longitude_deg, half_window_m)
    basemap = stitch_basemap(bounds, zoom, cache_dir)
    dem = sample_dem(dem_path, bounds, basemap.shape[:2])
    valid = dem[np.isfinite(dem)]
    if valid.size == 0:
        raise ValueError("DEM has no valid samples over the map bounds")

    ground_m = elevation_at(dem_path, latitude_deg, longitude_deg)
    west, south, east, north = bounds
    height, width = dem.shape
    x_grid, y_grid = np.meshgrid(
        np.linspace(west, east, width),
        np.linspace(north, south, height),
    )

    figure, axis = plt.subplots(figsize=(9.0, 8.0), dpi=150)
    extent = [west, east, south, north]
    axis.imshow(basemap, extent=extent, origin="upper", aspect="auto", zorder=1)
    contours = axis.contour(
        x_grid,
        y_grid,
        np.ma.masked_invalid(dem),
        levels=contour_levels(valid),
        colors="#1a1a1a",
        linewidths=1.15,
        alpha=0.9,
        zorder=4,
    )
    axis.clabel(contours, inline=True, fontsize=9, fmt="%d m")
    axis.plot(
        longitude_deg,
        latitude_deg,
        marker="x",
        markersize=14,
        markeredgewidth=2.5,
        color="#1f77b4",
        zorder=6,
        label="Drone location",
    )
    axis.annotate(
        f"ground {ground_m:.0f} m ASL",
        xy=(longitude_deg, latitude_deg),
        xytext=(12, 12),
        textcoords="offset points",
        color="#0b3d91",
        fontweight="bold",
        bbox={
            "boxstyle": "round,pad=0.35",
            "facecolor": "white",
            "edgecolor": "#1f77b4",
            "alpha": 0.92,
        },
        zorder=7,
    )
    axis.legend(loc="upper right", framealpha=0.9)
    axis.grid(True, linestyle="--", linewidth=0.4, color="red", alpha=0.2, zorder=5)
    axis.set(
        xlim=(west, east),
        ylim=(south, north),
        xlabel="Longitude",
        ylabel="Latitude",
        title=(
            f"Offline Context Map (WGS84) - +/-{half_window_m:.0f} m "
            f"from drone - elev {np.nanpercentile(valid, 2):.0f}-"
            f"{np.nanpercentile(valid, 98):.0f} m ASL\n"
            f"drone=({latitude_deg:.5f}, {longitude_deg:.5f})"
        ),
    )
    draw_scale_bar(axis, latitude_deg, bounds, half_window_m)
    figure.tight_layout()

    buffer = io.BytesIO()
    try:
        figure.savefig(buffer, format="png", dpi=150, bbox_inches="tight")
    finally:
        plt.close(figure)
    buffer.seek(0)
    with Image.open(buffer) as source:
        return source.convert("RGB").copy()
