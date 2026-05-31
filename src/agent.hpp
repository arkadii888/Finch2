#pragma once

#include <mutex>
#include <string>

#include "drone.hpp"

class LlmService;

class Agent {
 public:
    Agent(Drone& drone, LlmService& llm);

    void Run();
    std::string HandleUserInput(const std::string& prompt);

    std::string GetLastPrompt() const;
    std::string GetLastResponse() const;

 private:
    std::string RunLlmTurn(const std::string& prompt);

    Drone& drone_;
    LlmService& llm_;
    mutable std::mutex turn_mutex_;
    std::string last_prompt_;
    std::string last_response_;
};
