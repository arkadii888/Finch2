#pragma once

#include "node.hpp"

class SequenceNode : public Node {
 public:
    NodeStatus GetStatus() override;
    bool Validate() const override;
    std::string GetPrompt() const override;
};
