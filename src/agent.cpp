#include "agent.hpp"

#include <nlohmann/json.hpp>

Agent::Agent(Drone& drone) : drone_(drone) {}

void Agent::Run() {
    while (global_running) {
        std::cout << llm_output_.Get() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds {1});
    }
};

std::string Agent::GetDroneTelemetry() {
    nlohmann::json telemetry {drone_.GetTelemetry()};
    return telemetry.dump();
}

std::string Agent::GetOutput() {
    auto output = output_.Get();
    return !output.empty() ? output : "Not ready yet.";
}

void Agent::KillDrone() {
    drone_.Kill();
}

void Agent::ProcessInput(const std::string& input) {
    std::lock_guard lock {input_mutex_};

    if (llm_output_.Is_Processing()) {
        return;
    }

    auto output {std::async(std::launch::async, [this, input]{
        return llm_service_.Complete(input);
    })};

    llm_output_.Set(std::move(output));
}
