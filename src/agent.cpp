#include "agent.hpp"

#include <chrono>
#include <thread>

#include <nlohmann/json.hpp>

Agent::Agent(Drone& drone, LlmService& llm_service) : drone_ {drone}, llm_service_ {llm_service} {}

void Agent::Run() {
    while (global_running) {
        std::this_thread::sleep_for(std::chrono::seconds {1});
    }
}

std::string Agent::GetDroneTelemetry() {
    nlohmann::json telemetry {drone_.GetTelemetry()};
    return telemetry.dump();
}

std::string Agent::GetOutput() {
    if (is_processing_) {
        return "Processing...";
    }

    const std::string output {llm_output_.Get()};
    if (output.empty()) {
        return "Not ready yet.";
    }

    return output;
}

void Agent::KillDrone() {
    drone_.Kill();
}

void Agent::ProcessInput(const std::string& input) {
    bool expected {false};
    if (!is_processing_.compare_exchange_strong(expected, true)) {
        return;
    }

    std::thread([this, input] {
        HandleOutput(llm_service_.Complete(input));
        is_processing_ = false;
    }).detach();
}

void Agent::HandleOutput(std::string output) {
    llm_output_.Set(std::move(output));
}
