"""Rasterize a GeoPackage extract into a WGS84 basemap."""

from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
from PIL import Image

from map_drawing import (
    add_colored_polygons,
    add_lines,
    add_polygons,
    add_protected_areas,
    add_sports,
    add_trees,
    basemap_shape,
    clip_to_bounds,
    visible_road_classes,
    zoom_scale,
)
from map_labels import (
    feature_name,
    label_named_polygons,
    label_peaks,
    label_places,
    label_poi_area_names,
    label_pois,
    label_road_refs,
    label_roads,
    label_water,
)
from map_style import (
    FOREST_CLASSES,
    FOREST_EDGE,
    LANDUSE_COLORS,
    LANDUSE_LABEL_CLASSES,
    NATURAL_COLORS,
    PLACE_STYLES,
    ROAD_DRAW_ORDER,
    ROAD_STYLES,
    VECTOR_BACKGROUND,
    WATER_COLOR,
    WATER_OUTLINE,
    WETLAND_CLASSES,
    WETLAND_COLOR,
    WETLAND_OUTLINE,
    normalize_road_class,
)
from vector_io import features, list_vector_layers, vector_source, where


def render_vector_basemap(
    bounds: tuple[float, float, float, float],
    zoom: int,
    map_path: Path,
) -> np.ndarray:
    """Rasterize a Geofabrik GeoPackage extract at slippy-map zoom density."""
    if not (1 <= zoom <= 18):
        raise ValueError(f"Zoom must be between 1 and 18: {zoom}")

    source = vector_source(map_path)
    layers = list_vector_layers(source)
    if not layers:
        raise ValueError(f"No vector layers in map: {map_path}")

    west, south, east, north = bounds
    width, height = basemap_shape(bounds, zoom)
    scale = zoom_scale(zoom)
    dpi = 100.0
    figure, axis = plt.subplots(figsize=(width / dpi, height / dpi), dpi=dpi)
    axis.set_position((0.0, 0.0, 1.0, 1.0))
    axis.set_xlim(west, east)
    axis.set_ylim(south, north)
    axis.set_axis_off()
    axis.set_facecolor(VECTOR_BACKGROUND)
    figure.patch.set_facecolor(VECTOR_BACKGROUND)

    landuse_geoms, landuse_fields = features(source, layers, "landuse", bounds, ("fclass", "name"))
    if landuse_geoms is not None:
        landuse_classes = landuse_fields.get("fclass", [])
        add_colored_polygons(axis, landuse_geoms, landuse_classes, LANDUSE_COLORS, zorder=1)
        forests = where(landuse_geoms, landuse_classes, FOREST_CLASSES)
        if forests:
            add_polygons(
                axis, forests, facecolor="none", edgecolor=FOREST_EDGE, linewidth=0.45, zorder=1.2
            )

    natural_geoms, natural_fields = features(source, layers, "natural", bounds, ("fclass",))
    if natural_geoms is not None:
        add_colored_polygons(
            axis, natural_geoms, natural_fields.get("fclass", []), NATURAL_COLORS, zorder=2
        )

    protected_geoms, _ = features(source, layers, "protected", bounds)
    if protected_geoms is not None:
        add_protected_areas(axis, protected_geoms, bounds)

    tree_geoms, tree_fields = features(source, layers, "trees", bounds, ("fclass", "name"))
    if tree_geoms is not None:
        add_trees(axis, where(tree_geoms, tree_fields.get("fclass", []), "tree"), zoom)

    sport_geoms, sport_fields = features(source, layers, "poi_areas", bounds, ("fclass", "name"))
    if sport_geoms is not None:
        add_sports(axis, sport_geoms, sport_fields.get("fclass", []))

    water_geoms, water_fields = features(source, layers, "water", bounds, ("fclass", "name"))
    if water_geoms is not None:
        water_classes = water_fields.get("fclass", [])
        wetlands = where(water_geoms, water_classes, WETLAND_CLASSES)
        open_water = where(
            water_geoms, water_classes, {str(raw) for raw in water_classes} - WETLAND_CLASSES
        )
        if wetlands:
            add_polygons(
                axis,
                wetlands,
                facecolor=WETLAND_COLOR,
                edgecolor=WETLAND_OUTLINE,
                linewidth=0.4,
                zorder=3,
                hatch="....",
            )
        if open_water:
            add_polygons(
                axis,
                open_water,
                facecolor=WATER_COLOR,
                edgecolor=WATER_OUTLINE,
                linewidth=0.6,
                zorder=3.1,
            )
        label_water(axis, water_geoms, water_classes, water_fields.get("name"), zoom)

    way_geoms, way_fields = features(source, layers, "waterways", bounds, ("fclass", "name"))
    if way_geoms is not None:
        way_classes = way_fields.get("fclass", [])
        streams = where(way_geoms, way_classes, {str(raw) for raw in way_classes} - {"river"})
        rivers = where(way_geoms, way_classes, "river")
        if streams:
            add_lines(axis, streams, color="#8ebfd0", width=1.1 * scale, zorder=4)
        if rivers:
            add_lines(axis, rivers, color=WATER_OUTLINE, width=2.4 * scale, zorder=4)
            label_water(
                axis,
                rivers,
                ["river"] * len(rivers),
                where(way_fields.get("name", []), way_classes, "river"),
                zoom,
            )

    rail_geoms, _ = features(source, layers, "railways", bounds)
    if rail_geoms is not None:
        add_lines(axis, rail_geoms, color="#707070", width=1.6 * scale, zorder=5)
        add_lines(
            axis, rail_geoms, color="#ffffff", width=0.55 * scale, zorder=6, linestyle=(0, (3, 3))
        )

    road_geoms, road_fields = features(source, layers, "roads", bounds, ("fclass", "name", "ref"))
    if road_geoms is not None:
        road_class_values = road_fields.get("fclass", [])
        styled_road_classes = [normalize_road_class(value) for value in road_class_values]
        road_classes = visible_road_classes(zoom)
        for fclass in ROAD_DRAW_ORDER:
            group = where(road_geoms, styled_road_classes, fclass)
            style = ROAD_STYLES.get(fclass)
            if not group or style is None or fclass not in road_classes:
                continue
            casing, fill, casing_width, fill_width = style
            add_lines(axis, group, color=casing, width=casing_width * scale, zorder=7)
            add_lines(axis, group, color=fill, width=fill_width * scale, zorder=8)
        label_roads(axis, road_geoms, road_class_values, road_fields.get("name"), zoom)
        label_road_refs(
            axis, road_geoms, road_class_values, road_fields.get("ref"), zoom, bounds=bounds
        )

    if zoom >= 15:
        building_geoms, _ = features(source, layers, "buildings", bounds)
        if building_geoms is not None:
            add_polygons(
                axis,
                building_geoms,
                facecolor="#d9d0c9",
                edgecolor="#b9ada3",
                linewidth=0.35,
                zorder=9,
            )

    used_names: set[str] = set()
    if landuse_geoms is not None:
        used_names.update(
            label_named_polygons(
                axis,
                landuse_geoms,
                landuse_fields.get("fclass", []),
                landuse_fields.get("name"),
                LANDUSE_LABEL_CLASSES,
                zoom,
            )
        )

    place_geoms, place_fields = features(source, layers, "places", bounds, ("fclass", "name"))
    if place_geoms is not None:
        names = place_fields.get("name")
        used_names.update(
            label_places(
                axis,
                place_geoms,
                place_fields.get("fclass", []),
                names,
                zoom,
                used_names,
            )
        )

    area_geoms, area_fields = features(source, layers, "place_areas", bounds, ("fclass", "name"))
    area_names = area_fields.get("name")
    if area_geoms is not None and area_names is not None:
        window_area = max((east - west) * (north - south), 1e-18)
        clipped_geoms, clipped_classes, clipped_names = [], [], []
        area_classes = area_fields.get("fclass", [])
        for geom, raw_class, raw_name in zip(area_geoms, area_classes, area_names, strict=False):
            name = feature_name(raw_name)
            if name is None or name in used_names or str(raw_class) not in PLACE_STYLES:
                continue
            clipped = clip_to_bounds(geom, bounds)
            if clipped is None or clipped.area / window_area >= 0.85:
                continue
            clipped_geoms.append(clipped)
            clipped_classes.append(raw_class)
            clipped_names.append(name)
        if clipped_geoms:
            used_names.update(
                label_places(
                    axis,
                    clipped_geoms,
                    clipped_classes,
                    clipped_names,
                    zoom,
                    used_names,
                )
            )

    if tree_geoms is not None:
        label_peaks(
            axis,
            tree_geoms,
            tree_fields.get("fclass", []),
            tree_fields.get("name"),
            zoom,
            used_names,
        )

    poi_geoms, poi_fields = features(source, layers, "pois", bounds, ("fclass", "name"))
    if poi_geoms is not None:
        label_pois(
            axis,
            poi_geoms,
            poi_fields.get("fclass", []),
            poi_fields.get("name"),
            zoom,
            used_names,
        )

    if sport_geoms is not None:
        label_poi_area_names(
            axis,
            sport_geoms,
            sport_fields.get("fclass", []),
            sport_fields.get("name"),
            zoom,
            used_names,
        )

    axis.set_xlim(west, east)
    axis.set_ylim(south, north)
    figure.canvas.draw()
    image = np.asarray(figure.canvas.buffer_rgba())[:, :, :3].copy()
    plt.close(figure)
    if image.shape[1] != width or image.shape[0] != height:
        image = np.asarray(
            Image.fromarray(image).resize((width, height), Image.Resampling.BILINEAR)
        )
    return image
