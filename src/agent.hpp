#pragma once

#include <atomic>
#include <mutex>
#include <string>

#include "behavior_tree/btree.hpp"
#include "behavior_tree/node_catalog.hpp"
#include "vehicle/vehicle.hpp"
#include "llm_service/llm_service.hpp"

class LlmOutput {
 public:
    std::string Get() const {
        std::lock_guard lock {mutex_};
        return value_;
    }

    void Set(std::string value) {
        std::lock_guard lock {mutex_};
        value_ = std::move(value);
    }

 private:
    mutable std::mutex mutex_;
    std::string value_;
};

class Agent {
 public:
    Agent(Vehicle& vehicle, LlmService& llm_service);

    void Run();

    std::string GetVehicleTelemetry() const;
    std::string GetOutput();

    void KillVehicle();
    void ProcessInput(const std::string& input);

 private:
    std::string BuildSystemPrompt() const;
    void HandleOutput(std::string output);
    NodeStatus TickNode(Node* node);

    BTree btree_;
    Vehicle& vehicle_;
    LlmService& llm_service_;
    NodeCatalog node_catalog_;
    LlmOutput llm_output_;
    std::atomic<bool> is_processing_ {false};
};
