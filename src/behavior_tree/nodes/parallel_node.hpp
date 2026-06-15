#pragma once

#include "node.hpp"

class ParallelNode : public Node {
 public:
    explicit ParallelNode(int success_threshold);
    NodeStatus GetStatus() const override;
    bool Validate() const override;
    std::string GetPrompt() const override;

 private:
    int success_threshold_;
};
