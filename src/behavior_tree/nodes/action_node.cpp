#include "action_node.hpp"

NodeStatus ActionNode::GetStatus() const {
    return status_;
}

bool ActionNode::Validate() const {
    return true;
}

std::string ActionNode::GetPrompt() const {
    return "";
}
