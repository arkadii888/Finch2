#include "agent.hpp"

#include <nlohmann/json.hpp>

Agent::Agent(Drone& drone, LlmService& llm_service) : drone_(drone), llm_service_(llm_service) {}

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
    const LlmOutputSnapshot snapshot {llm_output_.Snapshot()};

    if (snapshot.state == LlmState::Ready) {
        return snapshot.result;
    }

    if (snapshot.state == LlmState::Running) {
        return "Processing...";
    }

    return "Not ready yet.";
}

void Agent::KillDrone() {
    drone_.Kill();
}

void Agent::ProcessInput(const std::string& input) {
    std::lock_guard lock {input_mutex_};

    if (llm_output_.IsRunning()) {
        return;
    }

    std::future<std::string> output {std::async(std::launch::async, [this, input] {
        return llm_service_.Complete(input);
    })};

    llm_output_.Set(std::move(output));
}
