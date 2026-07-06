#pragma once

#include "vehicle/vehicle.hpp"
#include "behavior_tree/nodes/action_node.hpp"

class MoveNode : public ActionNode {
 public:
    void Execute(std::any context) override;
    bool IsExecuted() const;

 protected:
    Vehicle* vehicle_ {nullptr};
    bool is_executed {false};
};
