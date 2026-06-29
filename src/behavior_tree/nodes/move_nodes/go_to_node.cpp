#include "go_to_node.hpp"

GoToNode::GoToNode(double latitude_deg, double longitude_deg, float absolute_altitude_m, float yaw_deg) : latitude_deg_(latitude_deg),
    longitude_deg_(longitude_deg), absolute_altitude_m_(absolute_altitude_m), yaw_deg_(yaw_deg) {}

void GoToNode::Execute(std::any context) {
    MoveNode::Execute(context);

    if (vehicle_) {
        vehicle_->GoTo(latitude_deg_, longitude_deg_, absolute_altitude_m_, yaw_deg_);
        status_ = NodeStatus::Running;
        is_executed = true;
    } else {
        status_ = NodeStatus::Failure;
    }
}

NodeStatus GoToNode::GetStatus() const {
    // TODO: check position in acceptance radius

    return status_;
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
    return R"(Command to move the drone to a specific location: {"type": "action", "go_to": {
        "latitude_deg": <degrees_double>,
        "longitude_deg": <degrees_double>,
        "absolute_altitude_m": <meters_float>,
        "yaw_deg": <degrees_float>}})";
}
