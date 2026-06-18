#pragma once

#include <string>

#include "behavior_tree/nodes/action_node.hpp"

class TaskNode : public ActionNode {
 public:
    virtual void Execute() = 0;

    NodeStatus GetStatus() const override;
    bool Validate() const override;
    std::string GetPrompt() const override;
};
