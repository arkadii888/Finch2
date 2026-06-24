#pragma once

#include "node.hpp"

class ActionNode : public Node {
 public:
    virtual void Execute() = 0;

    NodeStatus GetStatus() const override;
    bool Validate() const override;
    std::string GetPrompt() const override;
};
