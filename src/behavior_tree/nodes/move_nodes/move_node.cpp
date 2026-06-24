#include "move_node.hpp"

void MoveNode::Execute() {

}

NodeStatus MoveNode::GetStatus() const {
    return NodeStatus::Running;
}

bool MoveNode::Validate() const {
    return true;
}

std::string MoveNode::GetPrompt() const {
    return "";
}
