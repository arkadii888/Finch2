#pragma once

#include <chrono>
#include <memory>
#include <string>

#include <nlohmann/json.hpp>

#include "action_node.hpp"

class Task : public Action {
 public:
    explicit Task(std::string name);
    NodeStatus GetStatus() const final;

 protected:
    virtual NodeStatus Tick() const = 0;
};

class ComputeFibonacci : public Task {
 public:
    explicit ComputeFibonacci(int target_n);
    static std::string GetPrompt();
    static std::unique_ptr<Node> FromJson(const nlohmann::json& args);
    bool Validate() const override;

    int GetTargetN() const;

 protected:
    NodeStatus Tick() const override;

 private:
    static constexpr int kPauseMs {40};

    mutable bool started_ {false};
    mutable std::chrono::steady_clock::time_point start_time_ {};
    int target_n_;
};
