#pragma once

#include "node.hpp"

class SequenceNode : public Node {
 public:
    SequenceNode();
    static std::string GetPrompt();
    bool Validate() const override;
    NodeStatus GetStatus() const override;
};

class FallbackNode : public Node {
 public:
    FallbackNode();
    static std::string GetPrompt();
    bool Validate() const override;
    NodeStatus GetStatus() const override;
};

class ParallelNode : public Node {
 public:
    explicit ParallelNode(int success_threshold);
    static std::string GetPrompt();
    bool Validate() const override;
    NodeStatus GetStatus() const override;

 private:
    int success_threshold_;
};
