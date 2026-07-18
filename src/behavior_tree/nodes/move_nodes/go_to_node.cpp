#include "go_to_node.hpp"

#include <cmath>

#include "spdlog/spdlog.h"

namespace {

constexpr double kPi = 3.14159265358979323846;

// Initial bearing (forward azimuth) in degrees [0, 360) from one geo point to another.
float ComputeBearingDeg(double from_latitude_deg, double from_longitude_deg, double to_latitude_deg, double to_longitude_deg) {
    const double from_lat_rad {from_latitude_deg * kPi / 180.0};
    const double to_lat_rad {to_latitude_deg * kPi / 180.0};
    const double delta_lon_rad {(to_longitude_deg - from_longitude_deg) * kPi / 180.0};

    const double x {std::sin(delta_lon_rad) * std::cos(to_lat_rad)};
    const double y {std::cos(from_lat_rad) * std::sin(to_lat_rad)
        - std::sin(from_lat_rad) * std::cos(to_lat_rad) * std::cos(delta_lon_rad)};

    const double bearing_deg {std::fmod((std::atan2(x, y) * 180.0 / kPi) + 360.0, 360.0)};

    return static_cast<float>(bearing_deg);
}

}

GoToNode::GoToNode(double latitude_deg, double longitude_deg, float absolute_altitude_m, std::optional<float> yaw_deg) : latitude_deg_(latitude_deg),
    longitude_deg_(longitude_deg), absolute_altitude_m_(absolute_altitude_m), yaw_deg_(yaw_deg) {}

void GoToNode::Execute(std::any context) {
    MoveNode::Execute(context);

    if (vehicle_) {
        auto telemetry {vehicle_->GetTelemetry()};
        if (absolute_altitude_m_ - telemetry.home_absolute_altitude_m >= 120.f) {
            vehicle_->Rtl();
            spdlog::info("GoToNode::Execute: Emergency return (120m hit).");
            is_executed = true;
            return;
        }

        float yaw_deg {0.f};
        if (yaw_deg_.has_value()) {
            yaw_deg = *yaw_deg_;
        } else {
            yaw_deg = ComputeBearingDeg(telemetry.latitude_deg, telemetry.longitude_deg, latitude_deg_, longitude_deg_);
        }

        vehicle_->GoTo(latitude_deg_, longitude_deg_, absolute_altitude_m_, yaw_deg);
        is_executed = true;
    }
}

NodeStatus GoToNode::GetStatus() {
    if (status_ == NodeStatus::Success || status_ == NodeStatus::Failure) {
        return status_;
    }

    if (vehicle_ == nullptr) {
        status_ = NodeStatus::Failure;
        return status_;
    }

    auto telemetry {vehicle_->GetTelemetry()};

    const double PI = 3.14159265358979323846;

    double lat_mid_radians = ((longitude_deg_ + telemetry.longitude_deg) / 2.0) * PI / 180.0;

    double meters_per_deg_lat = 111132.0;
    double meters_per_deg_lon = 111132.0 * std::cos(lat_mid_radians);

    double d_lat_m = (latitude_deg_ - telemetry.latitude_deg) * meters_per_deg_lat;
    double d_lon_m = (longitude_deg_ - telemetry.longitude_deg) * meters_per_deg_lon;
    double d_alt_m = absolute_altitude_m_ - telemetry.absolute_altitude_m;

    double distanceSquared = (d_lat_m * d_lat_m) + (d_lon_m * d_lon_m) + (d_alt_m * d_alt_m);

    if (distanceSquared <= (globals::drone_acceptance_radius_m * globals::drone_acceptance_radius_m)) {
        status_ = NodeStatus::Success;
        return status_;
    }

    return status_;
}

bool GoToNode::Validate() const {
    if (latitude_deg_ < -90.0 || latitude_deg_ > 90.0) {
        return false;
    }
    if (longitude_deg_ < -180.0 || longitude_deg_ > 180.0) {
        return false;
    }
    if (yaw_deg_.has_value() && (*yaw_deg_ < 0.f || *yaw_deg_ > 360.f)) {
        return false;
    }
    return true;
}

std::string GoToNode::GetPrompt() const {
    return R"({"type": "action", "go_to": {
        "latitude_deg": <degrees_double>,
        "longitude_deg": <degrees_double>,
        "absolute_altitude_m": <meters_float>,
        "yaw_deg": <degrees_float, optional - omit to face the direction of travel>}})";
}
