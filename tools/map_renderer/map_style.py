"""Style constants for the offline GeoPackage basemap."""

VECTOR_BACKGROUND = "#f2efe9"

LANDUSE_COLORS = {
    "allotments": "#c9e1bf",
    "cemetery": "#aacbaf",
    "commercial": "#f2dad9",
    "farm": "#eef0d5",
    "farmland": "#eef0d5",
    "farmyard": "#f5dcba",
    "forest": "#add19e",
    "garden": "#cdebb0",
    "grass": "#cdebb0",
    "heath": "#d6d99f",
    "industrial": "#ebdbe8",
    "meadow": "#cdebb0",
    "military": "#f3d8d2",
    "nature_reserve": "#c8facc",
    "orchard": "#aedfa3",
    "park": "#c8facc",
    "quarry": "#c5c3c3",
    "recreation_ground": "#dffce2",
    "residential": "#e0dfdf",
    "retail": "#ffd6d1",
    "scrub": "#c8d7ab",
    "vineyard": "#aedfa3",
    "wood": "#add19e",
}
NATURAL_COLORS = {
    "beach": "#fff1ba",
    "glacier": "#dff1f5",
    "grassland": "#cdebb0",
    "heath": "#d6d99f",
    "scrub": "#c8d7ab",
    "water": "#aad3df",
    "wetland": "#c8dcb3",
    "wood": "#add19e",
}
WATER_COLOR = "#aad3df"
WATER_OUTLINE = "#7eafc2"
WETLAND_COLOR = "#c8dcb3"
WETLAND_OUTLINE = "#8fb48a"
WETLAND_CLASSES = {
    "wetland",
    "wetland_bog",
    "wetland_fen",
    "wetland_marsh",
    "wetland_reedbed",
    "wetland_swamp",
}
FOREST_CLASSES = {"forest", "wood"}
FOREST_EDGE = "#8fb878"
# Geofabrik does not store sport=*; small pitches are usually tennis/hard courts.
HARD_PITCH_MAX_AREA = 1.5e-7
PITCH_GRASS = "#90d679"
PITCH_GRASS_EDGE = "#6e9e55"
PITCH_HARD = "#d9a078"
PITCH_HARD_EDGE = "#c07858"
TRACK_COLOR = "#e8c4b0"
TRACK_EDGE = "#c49a86"
PLAYGROUND_COLOR = "#dffce2"
PLAYGROUND_EDGE = "#9ec9a4"
SWIMMING_POOL_COLOR = "#aad3df"
# casing, fill, casing width, fill width at zoom 15 (points ≈ OSM Carto px)
ROAD_STYLES = {
    "cycleway": ("#9bb8ea", "#9bb8ea", 1.2, 0.9),
    "footway": ("#fa8072", "#fa8072", 1.1, 0.8),
    "living_street": ("#9a9a9a", "#ededed", 3.4, 2.4),
    "motorway": ("#dc2a67", "#e892a2", 6.2, 4.6),
    "motorway_link": ("#dc2a67", "#e892a2", 4.0, 2.8),
    "path": ("#c2b280", "#c2b280", 1.1, 0.8),
    "pedestrian": ("#9a9a9a", "#dddde8", 3.6, 2.6),
    "primary": ("#a06b00", "#fcd6a4", 5.4, 4.0),
    "primary_link": ("#a06b00", "#fcd6a4", 3.6, 2.5),
    "residential": ("#9a9a9a", "#ffffff", 3.8, 2.8),
    "secondary": ("#707d05", "#f7fabf", 4.8, 3.5),
    "secondary_link": ("#707d05", "#f7fabf", 3.4, 2.4),
    "service": ("#b0b0b0", "#ffffff", 2.4, 1.6),
    "steps": ("#fa8072", "#fa8072", 1.1, 0.8),
    "tertiary": ("#8f8f8f", "#ffffff", 4.4, 3.2),
    "tertiary_link": ("#8f8f8f", "#ffffff", 3.2, 2.2),
    "track": ("#a38a5a", "#d8c19a", 1.8, 1.1),
    "trunk": ("#c84e2f", "#f9b29c", 5.8, 4.3),
    "trunk_link": ("#c84e2f", "#f9b29c", 3.8, 2.6),
    "unclassified": ("#9a9a9a", "#ffffff", 3.8, 2.8),
}
ROAD_DRAW_ORDER = (
    "track",
    "path",
    "footway",
    "steps",
    "cycleway",
    "service",
    "pedestrian",
    "living_street",
    "residential",
    "unclassified",
    "tertiary_link",
    "tertiary",
    "secondary_link",
    "secondary",
    "primary_link",
    "primary",
    "trunk_link",
    "trunk",
    "motorway_link",
    "motorway",
)
ROAD_LABEL_CLASSES = {
    "living_street",
    "motorway",
    "primary",
    "residential",
    "secondary",
    "tertiary",
    "trunk",
    "unclassified",
}
# Swiss-style yellow route shields (OSM Carto): Autobahn A13, Hauptstrasse 13, …
ROAD_REF_CLASSES = {
    "motorway",
    "primary",
    "secondary",
    "trunk",
}
ROAD_REF_SHIELD_FACE = "#f5c400"
ROAD_REF_SHIELD_EDGE = "#1a1a1a"
ROAD_REF_SHIELD_TEXT = "#1a1a1a"
PLACE_STYLES = {
    "city": (11.0, True),
    "farm": (6.5, False),
    "hamlet": (8.0, False),
    "island": (8.0, False),
    "locality": (7.0, False),
    "national_capital": (11.0, True),
    "suburb": (8.5, False),
    "town": (10.0, True),
    "village": (9.5, True),
}
POI_LABEL_CLASSES = {"castle", "fortress", "hospital", "ruins", "school"}
# Named historic polygons live in pois_a_free, not the point POI layer.
POI_AREA_LABEL_CLASSES = {"castle", "fortress", "ruins"}
PEAK_CLASSES = {"peak", "volcano"}
PEAK_LABEL_COLOR = "#734a08"
LANDUSE_LABEL_CLASSES = set(LANDUSE_COLORS)
PROTECTED_FILL = "#c8facc"
PROTECTED_EDGE = "#3d8a54"
VECTOR_LAYER_NEEDLES = {
    "buildings": ("buildings_a_free", "buildings_a", "buildings"),
    "landuse": ("landuse_a_free", "landuse_a", "landuse"),
    "natural": ("natural_a_free", "natural_a"),
    "place_areas": ("places_a_free",),
    "places": ("places_free", "places"),
    "poi_areas": ("pois_a_free",),
    "pois": ("pois_free",),
    "protected": ("protected_areas_a_free",),
    "railways": ("railways_free", "railways", "railway"),
    "roads": ("roads_free", "roads"),
    "trees": ("natural_free",),
    "water": ("water_a_free", "water_a"),
    "waterways": ("waterways_free", "waterways"),
}


def normalize_road_class(raw_class) -> str:
    """Map Geofabrik road variants onto renderer styles."""
    fclass = str(raw_class)
    return "track" if fclass.startswith("track_grade") else fclass
