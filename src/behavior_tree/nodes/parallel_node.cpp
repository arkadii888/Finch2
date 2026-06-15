#include "parallel_node.hpp"

ParallelNode::ParallelNode(int success_threshold)
    : Node {"parallel"}, success_threshold_ {success_threshold} {}

NodeStatus ParallelNode::GetStatus() const {
    int successes {0};
    int failures {0};
    for (const auto& child : children_) {
        const NodeStatus s {child->GetStatus()};
        if (s == NodeStatus::Success) ++successes;
        else if (s == NodeStatus::Failure) ++failures;
    }
    if (successes >= success_threshold_) {
        return NodeStatus::Success;
    }
    if (failures > static_cast<int>(children_.size()) - success_threshold_) {
        return NodeStatus::Failure;
    }
    return NodeStatus::Running;
}

bool ParallelNode::Validate() const {
    if (children_.empty()) {
        return false;
    }
    if (success_threshold_ < 1
            || success_threshold_ > static_cast<int>(children_.size())) {
        return false;
    }
    return Node::Validate();
}

std::string ParallelNode::GetPrompt() const {
    return R"(parallel: {"type": "parallel", "success_threshold": 1, "children": [...]})";
}
