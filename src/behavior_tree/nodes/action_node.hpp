#pragma once

#include "node.hpp"

class ActionNode : public Node {
 public:
    explicit ActionNode(std::string name);
    NodeStatus GetStatus() const override;
    bool Validate() const override;
    std::string GetPrompt() const override;
};
