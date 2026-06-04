#pragma once

#include <atomic>
#include <chrono>
#include <future>
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

    void Set(const std::string& value) {
        std::lock_guard lock {mutex_};
        value_ = value;
    }

 private:
    mutable std::mutex mutex_;
    std::string value_;
};

class LlmOutput {
 public:
    std::string Get() {
        std::lock_guard lock {mutex_};
        if (!value_.valid()) {
            return "";
        }
        if (value_.wait_for(std::chrono::milliseconds {0}) == std::future_status::ready) {
            is_processing_ = false;
            return value_.get();
        }
        return "";
    }

    bool Is_Processing() const {
        return is_processing_;
    }

    void Set(std::future<std::string> value) {
        std::lock_guard lock {mutex_};
        is_processing_ = true;
        value_ = std::move(value);
    }

 private:
    std::mutex mutex_;
    std::future<std::string> value_;
    std::atomic<bool> is_processing_ = false;
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
    LlmOutput llm_output_;
    LlmService llm_service_;
    Output output_;
    std::mutex input_mutex_;
};
