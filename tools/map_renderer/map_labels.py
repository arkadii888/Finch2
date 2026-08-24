"""Place, road, water, and POI labels for the vector basemap."""

import math

from matplotlib import patheffects

from geo import latlon_to_world_pixels
from map_drawing import clip_to_bounds, iter_lines
from map_style import (
    FOREST_CLASSES,
    PEAK_CLASSES,
    PEAK_LABEL_COLOR,
    PLACE_STYLES,
    POI_AREA_LABEL_CLASSES,
    POI_LABEL_CLASSES,
    ROAD_LABEL_CLASSES,
    ROAD_REF_CLASSES,
    ROAD_REF_SHIELD_EDGE,
    ROAD_REF_SHIELD_FACE,
    ROAD_REF_SHIELD_TEXT,
    WETLAND_CLASSES,
)


def feature_name(raw_name) -> str | None:
    name = str(raw_name).strip()
    if not name or name.lower() in {"nan", "none", "null"}:
        return None
    return name


def shorten_street(name: str) -> str:
    lower = name.lower()
    for suffix, short in (
        ("strasse", "str."),
        ("straße", "str."),
        ("gasse", "g."),
        ("platz", "pl."),
        ("brücke", "br."),
    ):
        if lower.endswith(suffix):
            return name[: len(name) - len(suffix)] + short
    return name


def label_size_scale(zoom: int) -> float:
    """Keep labels a constant fraction of the map when zoom only changes raster density."""
    return 2.0 ** (zoom - 15)


def label_angle(line, zoom: int) -> float:
    start = line.interpolate(0.42, normalized=True)
    end = line.interpolate(0.58, normalized=True)
    x0, y0 = latlon_to_world_pixels(start.y, start.x, zoom)
    x1, y1 = latlon_to_world_pixels(end.y, end.x, zoom)
    angle = math.degrees(math.atan2(y0 - y1, x1 - x0))
    if angle > 90.0:
        angle -= 180.0
    if angle < -90.0:
        angle += 180.0
    return angle


def label_point(
    axis,
    geom,
    name: str,
    fontsize: float,
    bold: bool = False,
    zorder: int = 12,
    color: str = "#1a1a1a",
    italic: bool = False,
    rotation: float = 0.0,
    zoom: int = 15,
) -> None:
    if geom is None or geom.is_empty:
        return
    scale = label_size_scale(zoom)
    point = geom if geom.geom_type == "Point" else geom.representative_point()
    text = axis.text(
        point.x,
        point.y,
        name,
        color=color,
        fontsize=fontsize * scale,
        fontweight="bold" if bold else "normal",
        fontstyle="italic" if italic else "normal",
        ha="center",
        va="center",
        rotation=rotation,
        rotation_mode="anchor",
        zorder=zorder,
    )
    text.set_path_effects(
        [patheffects.withStroke(linewidth=3.0 * scale, foreground="white")]
    )


def label_places(
    axis, geoms, classes, names, zoom: int, used_names: set[str] | None = None
) -> set[str]:
    if zoom < 13 or names is None:
        return set()
    blocked = used_names if used_names is not None else set()
    labeled = 0
    ranked = []
    for geom, raw_class, raw_name in zip(geoms, classes, names, strict=False):
        fclass = str(raw_class)
        style = PLACE_STYLES.get(fclass)
        name = feature_name(raw_name)
        if style is None or name is None or geom is None or geom.is_empty:
            continue
        if fclass in {"farm", "locality"} and zoom < 15:
            continue
        ranked.append((style[0], geom, name, style))
    ranked.sort(key=lambda item: item[0], reverse=True)
    used: set[str] = set()
    for _, geom, name, (fontsize, bold) in ranked:
        if labeled >= 20 or name in used or name in blocked:
            continue
        label_point(axis, geom, name, fontsize, bold, zorder=14, zoom=zoom)
        used.add(name)
        labeled += 1
    return used


def label_named_polygons(axis, geoms, classes, names, allowed: set[str], zoom: int) -> set[str]:
    if zoom < 14 or names is None:
        return set()
    labeled = 0
    used: set[str] = set()
    for geom, raw_class, raw_name in zip(geoms, classes, names, strict=False):
        if labeled >= 24:
            break
        name = feature_name(raw_name)
        fclass = str(raw_class)
        if (
            name is None
            or name in used
            or fclass not in allowed
            or geom is None
            or geom.area < 1e-7
        ):
            continue
        forestish = fclass in FOREST_CLASSES or fclass in {"cemetery", "park"}
        label_point(
            axis,
            geom,
            name,
            fontsize=7.5 if forestish else 7.0,
            italic=True,
            color="#3d6b32" if forestish else "#4a4a4a",
            zorder=11,
            zoom=zoom,
        )
        used.add(name)
        labeled += 1
    return used


def label_water(axis, geoms, classes, names, zoom: int) -> None:
    if zoom < 14 or names is None:
        return
    labeled = 0
    used: set[str] = set()
    for geom, raw_class, raw_name in zip(geoms, classes, names, strict=False):
        if labeled >= 8:
            break
        name = feature_name(raw_name)
        if name is None or name in used or geom is None or geom.is_empty:
            continue
        if geom.area > 0.0 and geom.area < 5e-7:
            continue
        italic = str(raw_class) in WETLAND_CLASSES
        label_point(
            axis,
            geom,
            name,
            fontsize=7.5,
            italic=True,
            color="#3d6b32" if italic else "#3d7a93",
            zorder=11,
            zoom=zoom,
        )
        used.add(name)
        labeled += 1


def label_pois(axis, geoms, classes, names, zoom: int, used: set[str]) -> None:
    if zoom < 15 or names is None:
        return
    labeled = 0
    for geom, raw_class, raw_name in zip(geoms, classes, names, strict=False):
        if labeled >= 12:
            break
        name = feature_name(raw_name)
        fclass = str(raw_class)
        if name is None or name in used:
            continue
        place_like = fclass == "tourist_info" and " " not in name and len(name) <= 22
        if fclass not in POI_LABEL_CLASSES and not place_like:
            continue
        fontsize = 7.0 if place_like else 6.5
        label_point(axis, geom, name, fontsize=fontsize, zorder=13, zoom=zoom)
        used.add(name)
        labeled += 1


def label_peaks(axis, geoms, classes, names, zoom: int, used: set[str]) -> None:
    """Label named OSM peaks even when a settlement already used the same name."""
    if zoom < 14 or names is None:
        return
    labeled = 0
    local_used: set[str] = set()
    for geom, raw_class, raw_name in zip(geoms, classes, names, strict=False):
        if labeled >= 16:
            break
        if str(raw_class) not in PEAK_CLASSES:
            continue
        name = feature_name(raw_name)
        if name is None or name in local_used or geom is None or geom.is_empty:
            continue
        label_point(
            axis,
            geom,
            name,
            fontsize=7.0,
            italic=True,
            color=PEAK_LABEL_COLOR,
            zorder=14,
            zoom=zoom,
        )
        local_used.add(name)
        used.add(name)
        labeled += 1


def label_poi_area_names(axis, geoms, classes, names, zoom: int, used: set[str]) -> None:
    """Label castle / ruins polygons that have no matching point POI."""
    if zoom < 14 or names is None:
        return
    labeled = 0
    for geom, raw_class, raw_name in zip(geoms, classes, names, strict=False):
        if labeled >= 12:
            break
        name = feature_name(raw_name)
        if (
            name is None
            or name in used
            or str(raw_class) not in POI_AREA_LABEL_CLASSES
            or geom is None
            or geom.is_empty
        ):
            continue
        label_point(axis, geom, name, fontsize=6.5, zorder=13, zoom=zoom)
        used.add(name)
        labeled += 1


def feature_ref(raw_ref) -> str | None:
    """Return a short route number (``A13``, ``13``). OSM may join several with ``;``."""
    ref = feature_name(raw_ref)
    if ref is None:
        return None
    ref = ref.split(";")[0].strip()
    if not ref or len(ref) > 8:
        return None
    return ref


def _draw_ref_shield(axis, geom, ref: str, zoom: int) -> None:
    """Draw an upright yellow rectangle like OSM Carto Swiss road shields."""
    if geom is None or geom.is_empty:
        return
    scale = label_size_scale(zoom)
    point = geom if geom.geom_type == "Point" else geom.representative_point()
    pad = 0.18 if len(ref) <= 3 else 0.12
    axis.text(
        point.x,
        point.y,
        ref,
        color=ROAD_REF_SHIELD_TEXT,
        fontsize=7.0 * scale,
        fontweight="bold",
        ha="center",
        va="center",
        zorder=15,
        bbox={
            "boxstyle": f"square,pad={pad}",
            "facecolor": ROAD_REF_SHIELD_FACE,
            "edgecolor": ROAD_REF_SHIELD_EDGE,
            "linewidth": 0.7 * scale,
        },
    )


def label_road_refs(axis, geoms, classes, refs, zoom: int, bounds=None) -> set[str]:
    """Place a few route-number shields per ``ref`` on major roads."""
    if zoom < 13 or refs is None:
        return set()
    by_ref: dict[str, list] = {}
    for geom, raw_class, raw_ref in zip(geoms, classes, refs, strict=False):
        if str(raw_class) not in ROAD_REF_CLASSES:
            continue
        ref = feature_ref(raw_ref)
        if ref is None:
            continue
        clipped = clip_to_bounds(geom, bounds) if bounds is not None else geom
        line = max(iter_lines(clipped), key=lambda item: item.length, default=None)
        if line is None or line.length < 0.000_18:
            continue
        by_ref.setdefault(ref, []).append(line)

    used: set[str] = set()
    placed = 0
    ranked = sorted(
        by_ref.items(),
        key=lambda item: sum(line.length for line in item[1]),
        reverse=True,
    )
    for ref, lines in ranked:
        if placed >= 16:
            break
        lines = sorted(lines, key=lambda line: line.length, reverse=True)
        longest = lines[0]
        fractions = (0.32, 0.68) if longest.length >= 0.003 else (0.5,)
        for fraction in fractions:
            if placed >= 16:
                break
            _draw_ref_shield(axis, longest.interpolate(fraction, normalized=True), ref, zoom)
            placed += 1
        used.add(ref)
    return used


def label_roads(axis, geoms, classes, names, zoom: int) -> None:
    if zoom < 15 or names is None:
        return
    candidates = []
    for geom, raw_class, raw_name in zip(geoms, classes, names, strict=False):
        name = feature_name(raw_name)
        if name is None or str(raw_class) not in ROAD_LABEL_CLASSES:
            continue
        line = max(iter_lines(geom), key=lambda item: item.length, default=None)
        if line is None or line.length < 0.000_12:
            continue
        candidates.append((line.length, line, shorten_street(name)))
    candidates.sort(key=lambda item: item[0], reverse=True)
    labeled = 0
    used: set[str] = set()
    for _, line, name in candidates:
        if labeled >= 36:
            break
        if name in used:
            continue
        point = line.interpolate(0.5, normalized=True)
        label_point(
            axis,
            point,
            name,
            fontsize=6.0,
            zorder=13,
            rotation=label_angle(line, zoom),
            zoom=zoom,
        )
        used.add(name)
        labeled += 1
