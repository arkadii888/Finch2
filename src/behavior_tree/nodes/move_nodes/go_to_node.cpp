#include "go_to_node.hpp"

#include <cmath>

GoToNode::GoToNode(double latitude_deg, double longitude_deg, float absolute_altitude_m, float yaw_deg) : latitude_deg_(latitude_deg),
    longitude_deg_(longitude_deg), absolute_altitude_m_(absolute_altitude_m), yaw_deg_(yaw_deg) {}

void GoToNode::Execute(std::any context) {
    MoveNode::Execute(context);

    if (vehicle_) {
        vehicle_->GoTo(latitude_deg_, longitude_deg_, absolute_altitude_m_, yaw_deg_);
        is_executed = true;
    }
}

NodeStatus GoToNode::GetStatus() const {
    if (vehicle_ == nullptr) {
        return NodeStatus::Failure;
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
        return NodeStatus::Success;
    }

    return NodeStatus::Running;
}

bool GoToNode::Validate() const {
    if (latitude_deg_ < -90.0 || latitude_deg_ > 90.0) {
        return false;
    }
    if (longitude_deg_ < -180.0 || longitude_deg_ > 180.0) {
        return false;
    }
    if (yaw_deg_ < 0.f || yaw_deg_ > 360.f) {
        return false;
    }
    return true;
}

std::string GoToNode::GetPrompt() const {
    return R"({"type": "action", "go_to": {
        "latitude_deg": <degrees_double>,
        "longitude_deg": <degrees_double>,
        "absolute_altitude_m": <meters_float>,
        "yaw_deg": <degrees_float>}})";
}
