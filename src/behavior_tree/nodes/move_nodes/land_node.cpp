#include "land_node.hpp"

void LandNode::Execute(std::any context) {
    MoveNode::Execute(context);

    if (vehicle_) {
        vehicle_->Land();
        is_executed = true;
    }
}

NodeStatus LandNode::GetStatus() {
    if (status_ == NodeStatus::Success || status_ == NodeStatus::Failure) {
        return status_;
    }

    if (vehicle_ == nullptr) {
        status_ = NodeStatus::Failure;
        return status_;
    }

    auto telemetry {vehicle_->GetTelemetry()};
    if (telemetry.is_armed) {
        return NodeStatus::Running;
    }

    status_ = NodeStatus::Success;
    return status_;
}

bool LandNode::Validate() const {
    return true;
}

std::string LandNode::GetPrompt() const {
    return R"({"type": "action", "land": {}})";
}
