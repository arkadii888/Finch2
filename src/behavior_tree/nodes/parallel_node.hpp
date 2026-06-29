#pragma once

#include "node.hpp"

class ParallelNode : public Node {
 public:
    explicit ParallelNode(int success_threshold);
    NodeStatus GetStatus() const override;
    bool Validate() const override;
    std::string GetPrompt() const override;

    int GetSuccessThreshold() const;

 private:
    int success_threshold_;
};
