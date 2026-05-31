#pragma once

#include <string>

#include "drone.hpp"
#include "llm_service.hpp"

class Agent {
 public:
    Agent(Drone& drone);

    void Run();
    std::string HandleUserInput(const std::string& prompt);

 private:
    Drone& drone_;
    LlmService llm_service_;
};
