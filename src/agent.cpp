#include "agent.hpp"

Agent::Agent(Drone& drone) : drone_(drone) {}

void Agent::Run() {

};

void Agent::SetPrompt(const std::string& prompt) {
    prompt_ = prompt;
}
