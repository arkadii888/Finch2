#pragma once

#include "behavior_tree/nodes/action_node.hpp"

class TaskNode : public ActionNode {
 public:
    void Execute(std::any context) override;
};
