#include "move_node.hpp"

void MoveNode::Execute(std::any context) {
    if (context.type() == typeid(Vehicle*)) {
        vehicle_ = std::any_cast<Vehicle*>(context);
    } else {
        vehicle_ = nullptr;
    }
}

bool MoveNode::IsExecuted() const {
    return is_executed;
}
