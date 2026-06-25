#include "move_node.hpp"

void MoveNode::Execute(std::any context) {
    if (context.type() == typeid(Vehicle*)) {
        vehicle_ = std::any_cast<Vehicle*>(context);
    } else {
        vehicle_ = nullptr;
    }
}

NodeStatus MoveNode::GetStatus() const {
    return status_;
}

bool MoveNode::Validate() const {
    return true;
}

std::string MoveNode::GetPrompt() const {
    return "";
}
