#include "takeoff_node.hpp"

#include <cstdlib>

TakeoffNode::TakeoffNode(float relative_altitude_m) : relative_altitude_m_(relative_altitude_m) {}

void TakeoffNode::Execute(std::any context) {
    MoveNode::Execute(context);

    if (vehicle_) {
        vehicle_->Takeoff(relative_altitude_m_);
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

    if (std::abs(telemetry.relative_altitude_m - relative_altitude_m_) > globals::drone_acceptance_radius_m) {
        return NodeStatus::Running;
    }

    status_ = NodeStatus::Success;
    return status_;
}

bool TakeoffNode::Validate() const {
    return relative_altitude_m_ > 0.f && relative_altitude_m_ <= 110.f;
}

std::string TakeoffNode::GetPrompt() const {
    return R"({"type": "action", "takeoff": {
        "relative_altitude_m": <meters_float, optional - omit to use the default takeoff altitude>}})";
}
