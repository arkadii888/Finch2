#include "sequence_node.hpp"

NodeStatus SequenceNode::GetStatus() const {
    return status_;
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
