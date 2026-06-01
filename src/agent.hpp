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

    std::string GetDroneTelemetry();
    std::string GetOutput();

    void KillDrone();
    void ProcessInput(const std::string& input);

 private:
    Drone& drone_;
    LlmService llm_service_;
};
