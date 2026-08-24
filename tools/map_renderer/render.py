"""Render an offline OSM and GeoTIFF context map for multimodal inference."""

import io
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
from matplotlib import patheffects
from PIL import Image

from geo import bbox_from_center, meters_to_degrees
from terrain import contour_levels, sample_dem
from vector_map import render_vector_basemap

IMAGE_SIZE = 1344
DPI = 96
GRID_SPACING_M = 200.0


def draw_scale_bar(
    axis,
    latitude_deg: float,
    bounds: tuple[float, float, float, float],
    half_window_m: float,
) -> None:
    """Draw a metric scale bar in the lower-left corner."""
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


def _centered_ticks(center: float, step: float, lo: float, hi: float) -> list[float]:
    values: list[float] = []
    value = center
    while value >= lo - 1e-12:
        values.append(value)
        value -= step
    value = center + step
    while value <= hi + 1e-12:
        values.append(value)
        value += step
    return values


def draw_metric_grid(
    axis,
    latitude_deg: float,
    longitude_deg: float,
    bounds: tuple[float, float, float, float],
) -> None:
    """Draw a faint fixed-meter grid through the map center."""
    west, south, east, north = bounds
    dlat, dlon = meters_to_degrees(latitude_deg, GRID_SPACING_M)
    style = {"color": "#444444", "linewidth": 0.4, "alpha": 0.35, "zorder": 3}
    for value in _centered_ticks(longitude_deg, dlon, west, east):
        axis.plot([value, value], [south, north], **style)
    for value in _centered_ticks(latitude_deg, dlat, south, north):
        axis.plot([west, east], [value, value], **style)


def render_snapshot(
    latitude_deg: float,
    longitude_deg: float,
    half_window_m: float,
    dem_path: Path,
    map_path: Path,
    zoom: int = 15,
) -> Image.Image:
    """Render the chrome-free square map consumed by the vision-language model."""
    if not dem_path.is_file():
        raise FileNotFoundError(f"DEM not found: {dem_path}")

    bounds = bbox_from_center(latitude_deg, longitude_deg, half_window_m)
    basemap = render_vector_basemap(bounds, zoom, map_path)
    dem = sample_dem(dem_path, bounds, basemap.shape[:2])
    valid = dem[np.isfinite(dem)]
    if valid.size == 0:
        raise ValueError("DEM has no valid samples over the map bounds")

    west, south, east, north = bounds
    height, width = dem.shape
    x_grid, y_grid = np.meshgrid(
        np.linspace(west, east, width),
        np.linspace(north, south, height),
    )
    extent = [west, east, south, north]

    figure = plt.figure(figsize=(IMAGE_SIZE / DPI, IMAGE_SIZE / DPI), dpi=DPI)
    axis = figure.add_axes((0.0, 0.0, 1.0, 1.0))
    axis.set_axis_off()
    axis.imshow(basemap, extent=extent, origin="upper", aspect="auto", zorder=1)
    axis.set_xlim(west, east)
    axis.set_ylim(south, north)
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
    axis.clabel(contours, inline=True, fontsize=11, fmt="%d m")
    draw_metric_grid(axis, latitude_deg, longitude_deg, bounds)
    axis.plot(
        longitude_deg,
        latitude_deg,
        marker="x",
        markersize=16,
        markeredgewidth=2.8,
        color="#1f77b4",
        zorder=6,
    )
    draw_scale_bar(axis, latitude_deg, bounds, half_window_m)

    buffer = io.BytesIO()
    try:
        figure.savefig(buffer, format="png", dpi=DPI, pad_inches=0)
    finally:
        plt.close(figure)
    buffer.seek(0)
    with Image.open(buffer) as source:
        image = source.convert("RGB")
        if image.size != (IMAGE_SIZE, IMAGE_SIZE):
            image = image.resize((IMAGE_SIZE, IMAGE_SIZE), Image.Resampling.NEAREST)
        return image.copy()
