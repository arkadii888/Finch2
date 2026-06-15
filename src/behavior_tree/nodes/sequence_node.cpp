#include "sequence_node.hpp"

SequenceNode::SequenceNode() : Node {"sequence"} {}

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

bool SequenceNode::Validate() const {
    if (children_.empty()) {
        return false;
    }
    return Node::Validate();
}

std::string SequenceNode::GetPrompt() const {
    return R"(sequence: {"type": "sequence", "children": [...]})";
}
