#pragma once

#include <string>
#include <atomic>

#include "drone.hpp"
#include "llm_service.hpp"

extern std::atomic<bool> global_running;

class Agent {
 public:
    Agent(Drone& drone);

    void Run();
    std::string HandleUserInput(const std::string& prompt);

 private:
    Drone& drone_;
    LlmService llm_service_ {"/home/arkadii/Documents/Projects/CxxProjects/Finch2/models/gemma-4-E2B-it-Q4_K_M.gguf"};
};
