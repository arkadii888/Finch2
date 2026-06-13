#pragma once

#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <string>

#include <nlohmann/json.hpp>

#include "action_node.hpp"

using TaskFactory = std::function<std::unique_ptr<Node>(const nlohmann::json&)>;

class TaskRegistry {
 public:
    static void Register(std::string key, TaskFactory factory);
    static TaskFactory* Find(const std::string& key);

 private:
    static std::map<std::string, TaskFactory>& entries();
};

struct TaskRegistrar {
    TaskRegistrar(std::string key, TaskFactory factory) {
        TaskRegistry::Register(std::move(key), std::move(factory));
    }
};

#define REGISTER_TASK_ACTION(key, Class) \
    static const TaskRegistrar Class##_task_registrar {key, &Class::FromJson};

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
    static NodeDescriptor Descriptor();
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
