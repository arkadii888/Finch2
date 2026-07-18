#pragma once

#include <chrono>

#include "task_node.hpp"

class FibNode : public TaskNode {
 public:
    explicit FibNode(int n);
    void Execute(std::any context) override;
    NodeStatus GetStatus() override;
    bool Validate() const override;
    std::string GetPrompt() const override;

 private:
    int n_;
    int index_ {1};
    long long a_ {0};
    long long b_ {1};
    bool started_ {false};
    std::chrono::steady_clock::time_point last_step_time_;
};
