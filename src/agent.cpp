#include "agent.hpp"

#include <chrono>
#include <thread>
#include <iostream>

Agent::Agent(Drone& drone) : drone_(drone) {}

void Agent::Run() {
    while (true) {
        auto telemetry {drone_.GetTelemetry()};
        std::cout << "Agent::Run(): Latitude is " << telemetry.latitude_deg << std::endl;
        std::cout << "Agent::Run(): Longitude is " << telemetry.longitude_deg << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds {1});
    }
};

void Agent::SetPrompt(const std::string& prompt) {
    prompt_.Set(prompt);
}
