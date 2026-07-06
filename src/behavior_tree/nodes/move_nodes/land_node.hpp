#pragma once

#include "move_node.hpp"

class LandNode : public MoveNode {
 public:
    void Execute(std::any context) override;
    NodeStatus GetStatus() override;
    bool Validate() const override;
    std::string GetPrompt() const override;
};
