#pragma once

#include <mutex>
#include <string>

#include "drone.hpp"

class PromptString {
 public:
    std::string Get() const {
        std::lock_guard<std::mutex> lock {mutex_};
        return value_;
    }

    void Set(const std::string& value) {
        std::lock_guard<std::mutex> lock {mutex_};
        value_ = value;
    }

 private:
    mutable std::mutex mutex_;
    std::string value_;
};

class Agent {
 public:
    Agent(Drone& drone);

    void Run();
    void SetPrompt(const std::string& prompt);

 private:
    Drone& drone_;
    PromptString prompt_;
};
