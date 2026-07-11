#include "rtl_node.hpp"

void RtlNode::Execute(std::any context) {
    MoveNode::Execute(context);

    if (vehicle_) {
        vehicle_->Rtl();
        is_executed = true;
    }
}

NodeStatus RtlNode::GetStatus() {
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

bool RtlNode::Validate() const {
    return true;
}

std::string RtlNode::GetPrompt() const {
    return R"({"type": "action", "rtl": {}})";
}
