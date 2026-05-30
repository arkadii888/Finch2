#include "agent.hpp"

Agent::Agent(Drone& drone) : drone_(drone) {}

void Agent::Run() {

};

void Agent::SetPrompt(const std::string& prompt) {
    std::lock_guard<std::mutex> lock {prompt_mutex_};
    prompt_ = prompt;
}
