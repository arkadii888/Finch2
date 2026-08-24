"""Geographic coordinate helpers for map rendering."""

import math

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


def latlon_to_world_pixels(
    latitude_deg: float,
    longitude_deg: float,
    zoom: int,
) -> tuple[float, float]:
    """Convert WGS84 coordinates to Web Mercator pixel space at ``zoom``."""
    latitude_deg = max(min(latitude_deg, 85.051_128_78), -85.051_128_78)
    scale = float((2**zoom) * TILE_SIZE)
    x = (longitude_deg + 180.0) / 360.0 * scale
    y = (1.0 - math.asinh(math.tan(math.radians(latitude_deg))) / math.pi) / 2.0 * scale
    return x, y
