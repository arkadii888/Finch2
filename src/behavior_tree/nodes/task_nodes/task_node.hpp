#pragma once

#include <string>

#include "behavior_tree/nodes/action_node.hpp"

class TaskNode : public ActionNode {
 public:
    void Execute() override;
    NodeStatus GetStatus() const override;
    bool Validate() const override;
    std::string GetPrompt() const override;
};
