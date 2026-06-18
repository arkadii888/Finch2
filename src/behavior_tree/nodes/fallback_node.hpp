#pragma once

#include "node.hpp"

class FallbackNode : public Node {
 public:
    NodeStatus GetStatus() const override;
    bool Validate() const override;
    std::string GetPrompt() const override;
};
