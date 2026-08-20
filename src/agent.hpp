#pragma once

#include <atomic>
#include <filesystem>
#include <mutex>
#include <string>
#include <thread>

#include "behavior_tree/btree.hpp"
#include "behavior_tree/node_catalog.hpp"
#include "llm_service/llm_service.hpp"
#include "map_image_service.hpp"
#include "runtime_config.hpp"
#include "vehicle/vehicle.hpp"

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
    Agent(Vehicle& vehicle, LlmService& llm_service, RuntimeConfig config);

    void Run();

    std::string GetVehicleTelemetry() const;
    std::string GetOutput();

    void ArmVehicle();
    void DisarmVehicle();
    void KillVehicle();
    void LandVehicle();
    void ReturnVehicle();

    bool ProcessInput(const std::string& input);

 private:
    std::string BuildSystemPrompt(const Telemetry& telemetry) const;
    void HandleOutput(std::string output, const std::filesystem::path& request_dir);
    void ProcessRequest(std::string input, Telemetry telemetry);
    NodeStatus TickNode(Node* node);

    BTree btree_;
    LlmOutput llm_output_;
    LlmService& llm_service_;
    NodeCatalog node_catalog_;
    Vehicle& vehicle_;
    RuntimeConfig config_;
    std::atomic<bool> is_processing_ {false};
    std::mutex btree_mutex_;
    std::jthread request_thread_;
};
