#pragma once

#include <mutex>
#include <string>

#include "drone.hpp"

class LlmService;

class Agent {
 public:
    Agent(Drone& drone, LlmService& llm);

    std::string GetLastPrompt() const;
    std::string GetLastResponse() const;
    std::string HandleUserInput(const std::string& prompt);
    void Run();

 private:
    std::string RunLlmTurn(const std::string& prompt);

    Drone& drone_;
    LlmService& llm_;
    std::string last_prompt_;
    std::string last_response_;
    mutable std::mutex turn_mutex_;
};
