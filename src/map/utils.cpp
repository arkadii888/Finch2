#include "utils.hpp"

#include <cmath>
#include <stdexcept>
#include <string>

namespace {

constexpr double METERS_PER_DEG_LAT {111'320.0};
constexpr double PI {3.14159265358979323846};
constexpr int PIXEL_MAX {1'000};

int RequirePixel(const nlohmann::json& value, const char* name) {
    if (!value.is_number_integer()) {
        throw std::runtime_error {
            std::string {name} + " must be an integer in [0, 1000]"
        };
    }
    const int pixel {value.get<int>()};
    if (pixel < 0 || pixel > PIXEL_MAX) {
        throw std::runtime_error {
            std::string {name} + " is outside [0, 1000]"
        };
    }
    return pixel;
}

void ConvertNode(nlohmann::json& node, const MapBounds& bounds, int& go_to_count, int max_go_to) {
    if (!node.is_object()) {
        return;
    }
    node.erase("n_go_to");

    if (node.contains("go_to")) {
        auto& go_to {node.at("go_to")};
        const int x {RequirePixel(go_to.at("x"), "x")};
        const int y {RequirePixel(go_to.at("y"), "y")};

        ++go_to_count;
        if (go_to_count > max_go_to) {
            throw std::runtime_error {
                "Behavior tree exceeds " + std::to_string(max_go_to)
                + " go_to waypoints"
            };
        }

        const double span_x {bounds.east - bounds.west};
        const double span_y {bounds.north - bounds.south};
        nlohmann::json converted {
            {
                "latitude_deg",
                bounds.north - (static_cast<double>(y) / PIXEL_MAX) * span_y
            },
            {
                "longitude_deg",
                bounds.west + (static_cast<double>(x) / PIXEL_MAX) * span_x
            },
            {"relative_altitude_m", go_to.at("relative_altitude_m")},
        };
        if (go_to.contains("reference_altitude_m")) {
            converted["reference_altitude_m"] = go_to.at("reference_altitude_m");
        }
        if (go_to.contains("yaw_deg")) {
            converted["yaw_deg"] = go_to.at("yaw_deg");
        }
        node["go_to"] = std::move(converted);
    }

    if (node.contains("children") && node.at("children").is_array()) {
        for (auto& child : node.at("children")) {
            ConvertNode(child, bounds, go_to_count, max_go_to);
        }
    }
}

}  // namespace

MapBounds MapBoundsFromCenter(
    const double latitude_deg,
    const double longitude_deg,
    const double half_window_m
) {
    if (!std::isfinite(half_window_m) || half_window_m <= 0.0) {
        throw std::runtime_error {"Map half-window must be positive"};
    }

    const double latitude_rad {latitude_deg * PI / 180.0};
    const double dlat {half_window_m / METERS_PER_DEG_LAT};
    const double dlon {
        half_window_m
        / (METERS_PER_DEG_LAT * std::max(std::cos(latitude_rad), 1e-6))
    };
    return {
        longitude_deg - dlon,
        latitude_deg - dlat,
        longitude_deg + dlon,
        latitude_deg + dlat,
    };
}

nlohmann::json MaterializeWgs84Btree(
    const nlohmann::json& pixel_tree,
    const MapBounds& bounds,
    const int max_go_to
) {
    if (max_go_to < 1) {
        throw std::runtime_error {"max_go_to must be at least 1"};
    }
    if (bounds.east == bounds.west || bounds.north == bounds.south) {
        throw std::runtime_error {"Map bounds have zero width or height"};
    }

    nlohmann::json tree = pixel_tree;
    int go_to_count {0};
    ConvertNode(tree, bounds, go_to_count, max_go_to);
    return tree;
}
