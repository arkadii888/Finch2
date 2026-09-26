#pragma once

#include <nlohmann/json.hpp>

// WGS84 square produced by the same 111320 m/deg formula as tools/map_renderer/geo.py.
class MapBounds {
 public:
    MapBounds(double latitude_deg, double longitude_deg, double half_window_m);

    double GetWest() const;
    void SetWest(double west);

    double GetSouth() const;
    void SetSouth(double south);

    double GetEast() const;
    void SetEast(double east);

    double GetNorth() const;
    void SetNorth(double north);

 private:
    void MapBoundsFromCenter(double latitude_deg, double longitude_deg, double half_window_m);

    double west_ {0.0};
    double south_ {0.0};
    double east_ {0.0};
    double north_ {0.0};
};

// Validates a flight-only pixel tree (at most max_go_to waypoints), strips n_go_to,
// and replaces each go_to x/y with WGS84 latitude_deg/longitude_deg.
nlohmann::json MaterializeWgs84Btree(
    const nlohmann::json& pixel_tree,
    const MapBounds& bounds,
    int max_go_to
);
