#pragma once

#include <atomic>
#include <chrono>
#include <future>
#include <mutex>
#include <string>

#include "drone/drone.hpp"
#include "llm_service/llm_service.hpp"

extern std::atomic<bool> global_running;

enum class LlmState {
    Idle,
    Ready,
    Running,
};

struct LlmOutputSnapshot {
    std::string result;
    LlmState state {LlmState::Idle};
};

class LlmOutput {
 public:
    bool IsRunning() const {
        std::lock_guard lock {mutex_};
        return state_ == LlmState::Running;
    }

    void Set(std::future<std::string> value) {
        std::lock_guard lock {mutex_};
        value_ = std::move(value);
        last_result_.clear();
        state_ = LlmState::Running;
    }

    LlmOutputSnapshot Snapshot() {
        std::lock_guard lock {mutex_};
        TryCompleteLocked();
        LlmOutputSnapshot snapshot {};
        snapshot.state = state_;
        if (state_ == LlmState::Ready) {
            snapshot.result = last_result_;
        }
        return snapshot;
    }

 private:
    void TryCompleteLocked() {
        if (state_ != LlmState::Running || !value_.valid()) {
            return;
        }

        if (value_.wait_for(std::chrono::milliseconds {0}) != std::future_status::ready) {
            return;
        }

        last_result_ = value_.get();
        value_ = std::future<std::string> {};
        state_ = LlmState::Ready;
    }

    std::string last_result_;
    mutable std::mutex mutex_;
    LlmState state_ {LlmState::Idle};
    std::future<std::string> value_;
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
    Drone& drone_;
    std::mutex input_mutex_;
    LlmOutput llm_output_;
    LlmService& llm_service_;
};
