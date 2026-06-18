#include "fallback_node.hpp"

NodeStatus FallbackNode::GetStatus() const {
    for (const auto& child : childrens_) {
        const NodeStatus s {child->GetStatus()};
        if (s == NodeStatus::Success) return NodeStatus::Success;
        if (s == NodeStatus::Running)  return NodeStatus::Running;
    }
    return NodeStatus::Failure;
}

bool FallbackNode::Validate() const {
    if (childrens_.empty()) {
        return false;
    }
    return true;
}

std::string FallbackNode::GetPrompt() const {
    return R"(fallback: {"type": "fallback", "children": [...]})";
}
