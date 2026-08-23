"""Shapely geometries to Matplotlib patches and lines."""

import numpy as np
import shapely
from matplotlib.collections import LineCollection, PatchCollection
from matplotlib.colors import to_rgba
from matplotlib.patches import PathPatch
from matplotlib.path import Path as MplPath
from shapely.errors import GEOSException

from geo import latlon_to_world_pixels
from map_style import (
    HARD_PITCH_MAX_AREA,
    PITCH_GRASS,
    PITCH_GRASS_EDGE,
    PITCH_HARD,
    PITCH_HARD_EDGE,
    PLAYGROUND_COLOR,
    PLAYGROUND_EDGE,
    PROTECTED_EDGE,
    PROTECTED_FILL,
    ROAD_STYLES,
    SWIMMING_POOL_COLOR,
    TRACK_COLOR,
    TRACK_EDGE,
    WATER_OUTLINE,
)


def iter_parts(geom, kinds: set[str]):
    if geom is None or geom.is_empty:
        return
    if geom.geom_type in kinds:
        yield geom
    elif geom.geom_type in {"GeometryCollection", "MultiLineString", "MultiPolygon"}:
        for part in geom.geoms:
            yield from iter_parts(part, kinds)


def iter_polygons(geom):
    yield from iter_parts(geom, {"Polygon"})


def iter_lines(geom):
    yield from iter_parts(geom, {"LineString", "LinearRing"})


def _ring_path_codes(coords: np.ndarray) -> np.ndarray:
    count = len(coords)
    codes = np.full(count, MplPath.LINETO, dtype=np.uint8)
    codes[0] = MplPath.MOVETO
    codes[-1] = MplPath.CLOSEPOLY
    return codes


def polygon_to_path(polygon):
    rings = [np.asarray(polygon.exterior.coords)]
    if len(rings[0]) < 4:
        return None
    codes = [_ring_path_codes(rings[0])]
    for interior in polygon.interiors:
        ring = np.asarray(interior.coords)
        if len(ring) < 4:
            continue
        rings.append(ring)
        codes.append(_ring_path_codes(ring))
    return MplPath(np.concatenate(rings), np.concatenate(codes))


def add_polygons(
    axis,
    geoms,
    facecolor,
    edgecolor="none",
    linewidth=0.0,
    zorder=1,
    hatch=None,
) -> None:
    patches = []
    for geom in geoms:
        for polygon in iter_polygons(geom):
            path = polygon_to_path(polygon)
            if path is not None:
                patches.append(PathPatch(path))
    if not patches:
        return
    axis.add_collection(
        PatchCollection(
            patches,
            facecolor=facecolor,
            edgecolor=edgecolor,
            linewidths=linewidth,
            hatch=hatch,
            zorder=zorder,
        )
    )


def add_lines(axis, geoms, color, width, zorder, linestyle="solid") -> None:
    segments = [
        np.asarray(line.coords)
        for geom in geoms
        for line in iter_lines(geom)
        if len(line.coords) >= 2
    ]
    if not segments:
        return
    axis.add_collection(
        LineCollection(
            segments,
            colors=color,
            linewidths=width,
            linestyles=linestyle,
            zorder=zorder,
            capstyle="round",
            joinstyle="round",
        )
    )


def add_colored_polygons(axis, geoms, classes, palette, zorder=1) -> None:
    grouped: dict[str, list] = {}
    for geom, raw_class in zip(geoms, classes, strict=False):
        color = palette.get(str(raw_class))
        if color is None:
            continue
        grouped.setdefault(color, []).append(geom)
    for color, group in grouped.items():
        add_polygons(axis, group, facecolor=color, zorder=zorder)


def add_sports(axis, geoms, classes) -> None:
    """Draw OSM pitches, tracks, playgrounds, and pools (Geofabrik polygon POIs)."""
    grass, hard, tracks, playgrounds, pools = [], [], [], [], []
    for geom, raw_class in zip(geoms, classes, strict=False):
        if geom is None or geom.is_empty:
            continue
        fclass = str(raw_class)
        if fclass == "pitch":
            (hard if geom.area < HARD_PITCH_MAX_AREA else grass).append(geom)
        elif fclass == "track":
            tracks.append(geom)
        elif fclass == "playground":
            playgrounds.append(geom)
        elif fclass == "swimming_pool":
            pools.append(geom)
    styles = (
        (playgrounds, PLAYGROUND_COLOR, PLAYGROUND_EDGE, 0.4, 2.6),
        (tracks, TRACK_COLOR, TRACK_EDGE, 0.5, 2.7),
        (grass, PITCH_GRASS, PITCH_GRASS_EDGE, 0.55, 2.8),
        (hard, PITCH_HARD, PITCH_HARD_EDGE, 0.55, 2.8),
        (pools, SWIMMING_POOL_COLOR, WATER_OUTLINE, 0.5, 3.2),
    )
    for group, fill, edge, width, zorder in styles:
        add_polygons(
            axis,
            group,
            facecolor=fill,
            edgecolor=edge,
            linewidth=width,
            zorder=zorder,
        )


def clip_to_bounds(geom, bounds: tuple[float, float, float, float]):
    if geom is None or geom.is_empty:
        return None
    clip = shapely.box(*bounds)
    try:
        clipped = geom.intersection(clip)
    except (GEOSException, ValueError):
        try:
            clipped = geom.buffer(0).intersection(clip)
        except (GEOSException, ValueError):
            return None
    if clipped is None or clipped.is_empty:
        return None
    return clipped


def add_protected_areas(axis, geoms, bounds: tuple[float, float, float, float]) -> None:
    """Draw nature reserves as a light green fill and outline, clipped to the window."""
    fills = []
    outlines = []
    west, south, east, north = bounds
    window_area = max((east - west) * (north - south), 1e-18)
    for geom in geoms:
        clipped = clip_to_bounds(geom, bounds)
        if clipped is None:
            continue
        outlines.append(clipped)
        if clipped.area / window_area < 0.5:
            fills.append(clipped)
    add_polygons(
        axis,
        fills,
        facecolor=(*to_rgba(PROTECTED_FILL)[:3], 0.22),
        edgecolor=PROTECTED_EDGE,
        linewidth=0.0,
        zorder=2.1,
        hatch="....",
    )
    add_polygons(
        axis,
        outlines,
        facecolor="none",
        edgecolor=PROTECTED_EDGE,
        linewidth=1.1,
        zorder=2.2,
    )


def zoom_scale(zoom: int) -> float:
    return max(0.5, min(1.4, 2 ** ((zoom - 15) * 0.35)))


def visible_road_classes(zoom: int) -> set[str]:
    visible = {"motorway", "motorway_link", "primary", "primary_link", "trunk", "trunk_link"}
    if zoom >= 13:
        visible.update({"secondary", "secondary_link"})
    if zoom >= 14:
        visible.update({"residential", "tertiary", "tertiary_link", "unclassified"})
    if zoom >= 15:
        visible.update(ROAD_STYLES)
    return visible


def add_trees(axis, geoms, zoom: int) -> None:
    if zoom < 15:
        return
    xs: list[float] = []
    ys: list[float] = []
    for geom in geoms:
        if geom is None or geom.is_empty or geom.geom_type != "Point":
            continue
        xs.append(geom.x)
        ys.append(geom.y)
    if not xs:
        return
    axis.scatter(
        xs,
        ys,
        s=3.5,
        c="#5e8c4a",
        marker="^",
        linewidths=0,
        alpha=0.75,
        zorder=2.4,
    )


def basemap_shape(bounds: tuple[float, float, float, float], zoom: int) -> tuple[int, int]:
    west, south, east, north = bounds
    x0, y_north = latlon_to_world_pixels(north, west, zoom)
    x1, y_south = latlon_to_world_pixels(south, east, zoom)
    width = max(64, min(2_048, int(round(abs(x1 - x0)))))
    height = max(64, min(2_048, int(round(abs(y_south - y_north)))))
    return width, height
