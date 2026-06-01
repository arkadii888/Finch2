#include "agent.hpp"

#include <chrono>
#include <thread>

#include <nlohmann/json.hpp>

Agent::Agent(Drone& drone) : drone_(drone) {}

void Agent::Run() {
    while (global_running) {
        std::this_thread::sleep_for(std::chrono::seconds {1});
    }
};

std::string Agent::GetDroneTelemetry() {
    nlohmann::json telemetry {drone_.GetTelemetry()};
    return telemetry.dump();
}

std::string Agent::GetOutput() {
    return "Not ready yet.";
}

void Agent::KillDrone() {
    drone_.Kill();
}

void Agent::ProcessInput(const std::string& input) {

}
