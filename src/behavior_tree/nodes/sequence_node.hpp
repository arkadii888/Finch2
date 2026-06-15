#pragma once

#include "node.hpp"

class SequenceNode : public Node {
 public:
    SequenceNode();
    NodeStatus GetStatus() const override;
    bool Validate() const override;
    std::string GetPrompt() const override;
};
