"""DEM sampling and contour helpers."""

import math
from pathlib import Path

import numpy as np
import rasterio
from rasterio.transform import from_bounds
from rasterio.warp import Resampling, reproject


def sample_dem(
    dem_path: Path,
    bounds: tuple[float, float, float, float],
    output_shape: tuple[int, int],
) -> np.ndarray:
    """Reproject a DEM onto the rendered WGS84 map grid."""
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
