#include "task_node.hpp"

NodeStatus TaskNode::GetStatus() const {
    return NodeStatus::Running;
}

bool TaskNode::Validate() const {
    return true;
}

std::string TaskNode::GetPrompt() const {
    return "";
}
