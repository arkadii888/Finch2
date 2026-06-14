#include "composite_nodes.hpp"

SequenceNode::SequenceNode() : Node {"sequence"} {}

std::string SequenceNode::GetPrompt() {
    return R"(sequence: {"type": "sequence", "children": [...]})";
}

bool SequenceNode::Validate() const {
    if (children_.empty()) {
        return false;
    }
    return Node::Validate();
}

NodeStatus SequenceNode::GetStatus() const {
    for (const auto& child : children_) {
        const NodeStatus s {child->GetStatus()};
        if (s == NodeStatus::Failure) {
            return NodeStatus::Failure;
        }
        if (s == NodeStatus::Running) { 
            return NodeStatus::Running;
        }
    }
    return NodeStatus::Success;
}

FallbackNode::FallbackNode() : Node {"fallback"} {}

std::string FallbackNode::GetPrompt() {
    return R"(fallback: {"type": "fallback", "children": [...]})";
}

bool FallbackNode::Validate() const {
    if (children_.empty()) {
        return false;
    }
    return Node::Validate();
}

NodeStatus FallbackNode::GetStatus() const {
    for (const auto& child : children_) {
        const NodeStatus s {child->GetStatus()};
        if (s == NodeStatus::Success) return NodeStatus::Success;
        if (s == NodeStatus::Running)  return NodeStatus::Running;
    }
    return NodeStatus::Failure;
}

ParallelNode::ParallelNode(int success_threshold)
    : Node {"parallel"}, success_threshold_ {success_threshold} {}

std::string ParallelNode::GetPrompt() {
    return R"(parallel: {"type": "parallel", "success_threshold": 1, "children": [...]})";
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
