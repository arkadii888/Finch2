#include "takeoff_node.hpp"

#include <cstdlib>

import globals;

void TakeoffNode::Execute(std::any context) {
    if (vehicle_) {
        vehicle_->Takeoff();
        is_executed = true;
    }
}

NodeStatus TakeoffNode::GetStatus() const {
    if (vehicle_ == nullptr) {
        return NodeStatus::Failure;
    }

    auto telemetry {vehicle_->GetTelemetry()};

    if (std::abs(telemetry.relative_altitude_m - globals::drone_takeoff_altitude_m) > globals::drone_acceptance_radius_m) {
        return NodeStatus::Running;
    }

    return NodeStatus::Success;
}

bool TakeoffNode::Validate() const {
    return true;
}

std::string TakeoffNode::GetPrompt() const {
    return R"({"type": "action", "takeoff": {}})";
}
