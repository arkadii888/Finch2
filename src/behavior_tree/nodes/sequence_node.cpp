#include "sequence_node.hpp"

NodeStatus SequenceNode::GetStatus() const {
    for (const auto& child : childrens_) {
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

bool SequenceNode::Validate() const {
    if (childrens_.empty()) {
        return false;
    }
    return true;
}

std::string SequenceNode::GetPrompt() const {
    return R"(sequence: {"type": "sequence", "children": [...]})";
}
