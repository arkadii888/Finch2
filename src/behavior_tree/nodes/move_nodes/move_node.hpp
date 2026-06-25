#pragma once

#include <string>

#include "vehicle/vehicle.hpp"
#include "behavior_tree/nodes/action_node.hpp"

class MoveNode : public ActionNode {
 public:
    void Execute(std::any context) override;
    NodeStatus GetStatus() const override;
    bool Validate() const override;
    std::string GetPrompt() const override;

 protected:
    Vehicle* vehicle_ {nullptr};
};
