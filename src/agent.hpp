#pragma once

#include <mutex>
#include <string>

#include "drone.hpp"

class Agent {
 public:
    Agent(Drone& drone);

    void Run();
    void SetPrompt(const std::string& prompt);

 private:
    Drone& drone_;
    std::string prompt_;
    std::mutex prompt_mutex_;
};
