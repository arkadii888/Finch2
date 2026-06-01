#pragma once

#include <mutex>
#include <string>
#include <atomic>

#include "drone.hpp"
#include "llm_service.hpp"

extern std::atomic<bool> global_running;

class Output {
 public:
    void Set(const std::string& value) {
        std::lock_guard lock {mutex_};
        value_ = value;
    }

    std::string Get() const {
        std::lock_guard lock {mutex_};
        return value_;
    }

 private:
    mutable std::mutex mutex_;
    std::string value_;
};

class Agent {
 public:
    Agent(Drone& drone);

    void Run();

    std::string GetDroneTelemetry();
    std::string GetOutput();

    void KillDrone();
    void ProcessInput(const std::string& input);

 private:
    Drone& drone_;
    LlmService llm_service_;
    Output output_;
};
