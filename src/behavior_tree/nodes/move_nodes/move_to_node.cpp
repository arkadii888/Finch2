#include "move_to_node.hpp"

MoveToNode::MoveToNode(double latitude_deg, double longitude_deg, float relative_altitude_m) :
    latitude_deg_ {latitude_deg}, longitude_deg_ {longitude_deg}, relative_altitude_m_ {relative_altitude_m} {}

std::vector<MissionItem> MoveToNode::GetMissionItems() const {
    MissionItem item {};
    item.latitude_deg = latitude_deg_;
    item.longitude_deg = longitude_deg_;
    item.relative_altitude_m = relative_altitude_m_;
    item.is_fly_through = false;
    return {item};
}

NodeStatus MoveToNode::GetStatus() const {
    return NodeStatus::Running;
}

bool MoveToNode::Validate() const {
    if (latitude_deg_ < -90.0 || latitude_deg_ > 90.0) {
        return false;
    }
    if (longitude_deg_ < -180.0 || longitude_deg_ > 180.0) {
        return false;
    }
    return true;
}

std::string MoveToNode::GetPrompt() const {
    return R"(Command to move the drone to a specific location: {"type": "action", "move_to": {
        "latitude_deg": <degrees_double>,
        "longitude_deg": <degrees_double>,
        "relative_altitude_m": <meters_float>}})";
}
