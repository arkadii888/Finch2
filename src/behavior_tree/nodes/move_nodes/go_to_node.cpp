#include "go_to_node.hpp"

#include <cmath>

namespace {

constexpr double PI = 3.14159265358979323846;
constexpr double METERS_PER_DEGREE_LATITUDE = 111132.0;
constexpr double MIN_BEARING_DISTANCE_M = 0.001;

struct LocalOffsetMeters {
    double north;
    double east;
    double HorizontalDistanceSquared() const { return (north * north) + (east * east); }
};

LocalOffsetMeters ComputeLocalOffsetMeters(double from_latitude_deg,
    double from_longitude_deg,
    double to_latitude_deg,
    double to_longitude_deg) {
    const double delta_longitude_deg {std::remainder(to_longitude_deg - from_longitude_deg, 360.0)};
    const double mean_latitude_rad {((from_latitude_deg + to_latitude_deg) / 2.0) * PI / 180.0};

    return {
        (to_latitude_deg - from_latitude_deg) * METERS_PER_DEGREE_LATITUDE,
        delta_longitude_deg * METERS_PER_DEGREE_LATITUDE * std::cos(mean_latitude_rad),
    };
}

float ComputeBearingDeg(double from_latitude_deg,
    double from_longitude_deg,
    double to_latitude_deg,
    double to_longitude_deg,
    float current_yaw_deg) {
    const auto offset {
        ComputeLocalOffsetMeters(from_latitude_deg, from_longitude_deg, to_latitude_deg, to_longitude_deg)};
    if (offset.HorizontalDistanceSquared() <= (MIN_BEARING_DISTANCE_M * MIN_BEARING_DISTANCE_M)) {
        return current_yaw_deg;
    }

    const double from_latitude_rad {from_latitude_deg * PI / 180.0};
    const double to_latitude_rad {to_latitude_deg * PI / 180.0};
    const double delta_longitude_rad {(to_longitude_deg - from_longitude_deg) * PI / 180.0};

    const double x {std::sin(delta_longitude_rad) * std::cos(to_latitude_rad)};
    const double y {std::cos(from_latitude_rad) * std::sin(to_latitude_rad)
        - std::sin(from_latitude_rad) * std::cos(to_latitude_rad) * std::cos(delta_longitude_rad)};

    return static_cast<float>(std::fmod((std::atan2(x, y) * 180.0 / PI) + 360.0, 360.0));
}

}

GoToNode::GoToNode(double latitude_deg, double longitude_deg, float absolute_altitude_m, std::optional<float> yaw_deg) : latitude_deg_(latitude_deg),
    longitude_deg_(longitude_deg), absolute_altitude_m_(absolute_altitude_m), yaw_deg_(yaw_deg) {}

void GoToNode::Execute(std::any context) {
    MoveNode::Execute(context);

    if (vehicle_) {
        float yaw_deg {0.f};
        if (yaw_deg_.has_value()) {
            yaw_deg = *yaw_deg_;
        } else {
            auto telemetry {vehicle_->GetTelemetry()};
            yaw_deg = ComputeBearingDeg(
                telemetry.latitude_deg, telemetry.longitude_deg, latitude_deg_, longitude_deg_, telemetry.yaw_deg);
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

    const auto offset {
        ComputeLocalOffsetMeters(telemetry.latitude_deg, telemetry.longitude_deg, latitude_deg_, longitude_deg_)};
    const double d_alt_m {absolute_altitude_m_ - telemetry.absolute_altitude_m};
    const double distance_squared {offset.HorizontalDistanceSquared() + (d_alt_m * d_alt_m)};

    if (distance_squared <= (globals::drone_acceptance_radius_m * globals::drone_acceptance_radius_m)) {
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
