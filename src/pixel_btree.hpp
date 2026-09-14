#pragma once

#include <nlohmann/json.hpp>

// WGS84 square produced by the same 111320 m/deg formula as tools/map_renderer/geo.py.
struct MapBounds {
    double west {0.0};
    double south {0.0};
    double east {0.0};
    double north {0.0};
};

MapBounds MapBoundsFromCenter(
    double latitude_deg,
    double longitude_deg,
    double half_window_m
);

// Validates a flight-only pixel tree (at most max_go_to waypoints), strips n_go_to,
// and replaces each go_to x/y with WGS84 latitude_deg/longitude_deg.
nlohmann::json MaterializeWgs84Btree(
    const nlohmann::json& pixel_tree,
    const MapBounds& bounds,
    int max_go_to
);
