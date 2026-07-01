#include "land_node.hpp"

void LandNode::Execute(std::any context) {
    MoveNode::Execute(context);

    if (vehicle_) {
        vehicle_->Land();
        is_executed = true;
    }
}

NodeStatus LandNode::GetStatus() const {
    if (vehicle_ == nullptr) {
        return NodeStatus::Failure;
    }

    auto telemetry {vehicle_->GetTelemetry()};
    if (telemetry.is_armed) {
        return NodeStatus::Running;
    }

    return NodeStatus::Success;
}

bool LandNode::Validate() const {
    return true;
}

std::string LandNode::GetPrompt() const {
    return R"({"type": "action", "land": {}})";
}
