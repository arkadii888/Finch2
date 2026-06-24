#include "go_to_node.hpp"

GoToNode::GoToNode(double latitude_deg, double longitude_deg, float absolute_altitude_m, float yaw_deg) : latitude_deg_(latitude_deg),
    longitude_deg_(longitude_deg), absolute_altitude_m_(absolute_altitude_m), yaw_deg_(yaw_deg) {}

void GoToNode::Execute() {

}

NodeStatus GoToNode::GetStatus() const {

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
