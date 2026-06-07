#pragma once

#include <atomic>
#include <mutex>
#include <string>

#include "drone/drone.hpp"
#include "llm_service/llm_service.hpp"

extern std::atomic<bool> global_running;

class Output {
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
    Agent(Drone& drone, LlmService& llm_service);

    void Run();

    std::string GetDroneTelemetry();
    std::string GetOutput();

    void KillDrone();
    void ProcessInput(const std::string& input);

 private:
    void HandleOutput(std::string output);

    Drone& drone_;
    LlmService& llm_service_;
    Output llm_output_;
    std::atomic<bool> is_processing_ {false};
};
