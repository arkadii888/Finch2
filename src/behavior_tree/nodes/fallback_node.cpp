#include "fallback_node.hpp"

NodeStatus FallbackNode::GetStatus() const {
    return status_;
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
