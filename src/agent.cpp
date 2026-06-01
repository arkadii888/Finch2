#include "agent.hpp"

#include <chrono>
#include <thread>

Agent::Agent(Drone& drone) : drone_(drone) {}

void Agent::Run() {
    while (global_running) {
        std::this_thread::sleep_for(std::chrono::seconds {1});
    }
};

std::string Agent::HandleUserInput(const std::string& prompt) {
    return llm_service_.Complete(prompt);
}
