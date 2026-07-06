#include "takeoff_node.hpp"

#include <cstdlib>

void TakeoffNode::Execute(std::any context) {
    MoveNode::Execute(context);

    if (vehicle_) {
        vehicle_->Takeoff();
        is_executed = true;
    }
}

NodeStatus TakeoffNode::GetStatus() {
    if (status_ == NodeStatus::Success || status_ == NodeStatus::Failure) {
        return status_;
    }

    if (vehicle_ == nullptr) {
        status_ = NodeStatus::Failure;
        return status_;
    }

    auto telemetry {vehicle_->GetTelemetry()};

    if (std::abs(telemetry.relative_altitude_m - globals::drone_takeoff_altitude_m) > globals::drone_acceptance_radius_m) {
        return NodeStatus::Running;
    }

    status_ = NodeStatus::Success;
    return status_;
}

bool TakeoffNode::Validate() const {
    return true;
}

std::string TakeoffNode::GetPrompt() const {
    return R"({"type": "action", "takeoff": {}})";
}
