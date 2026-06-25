#pragma once

#include <string>

#include "behavior_tree/nodes/action_node.hpp"

class TaskNode : public ActionNode {
 public:
    void Execute(std::any context) override;
    NodeStatus GetStatus() const override;
    bool Validate() const override;
    std::string GetPrompt() const override;
};
