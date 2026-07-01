#pragma once

#include "move_node.hpp"

import globals;

class TakeoffNode : public MoveNode {
 public:
    void Execute(std::any context) override;
    NodeStatus GetStatus() const override;
    bool Validate() const override;
    std::string GetPrompt() const override;
};
