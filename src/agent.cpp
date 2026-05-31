#include "agent.hpp"

#include <chrono>
#include <thread>
#include <iostream>

#include "llm_service.hpp"

Agent::Agent(Drone& drone, LlmService& llm) : drone_ {drone}, llm_ {llm} {}

void Agent::Run() {
    while (true) {
        auto telemetry {drone_.GetTelemetry()};
        std::cout << "Agent::Run(): Latitude is " << telemetry.latitude_deg << std::endl;
        std::cout << "Agent::Run(): Longitude is " << telemetry.longitude_deg << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds {1});
    }
}

std::string Agent::HandleUserInput(const std::string& prompt) {
    return RunLlmTurn(prompt);
}

std::string Agent::GetLastPrompt() const {
    std::lock_guard<std::mutex> lock {turn_mutex_};
    return last_prompt_;
}

std::string Agent::GetLastResponse() const {
    std::lock_guard<std::mutex> lock {turn_mutex_};
    return last_response_;
}

std::string Agent::RunLlmTurn(const std::string& prompt) {
    const std::string response {llm_.Complete(prompt)};

    {
        std::lock_guard<std::mutex> lock {turn_mutex_};
        last_prompt_ = prompt;
        last_response_ = response;
    }

    return response;
}
