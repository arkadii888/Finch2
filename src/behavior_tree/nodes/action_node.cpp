#include "action_node.hpp"

ActionNode::ActionNode(std::string name) : Node {std::move(name)} {}

NodeStatus ActionNode::GetStatus() const {
    return NodeStatus::Running;
}

bool ActionNode::Validate() const {
    return true;
}

std::string ActionNode::GetPrompt() const {
    return "";
}
