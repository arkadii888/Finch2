#include "agent.hpp"

#include <chrono>
#include <thread>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

import lifecycle;

Agent::Agent(Vehicle& vehicle, LlmService& llm_service)
    : vehicle_ {vehicle}, llm_service_ {llm_service} {}

void Agent::Run() {
    while (lifecycle::is_alive_public) {
        if(!llm_output_.Get().empty()) {
            WalkOnTree(btree_.GetRoot());
        }
        std::this_thread::sleep_for(std::chrono::seconds {1});
    }
}

std::string Agent::GetVehicleTelemetry() const {
    nlohmann::json telemetry {vehicle_.GetTelemetry()};
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

void Agent::KillVehicle() {
    vehicle_.Kill();
}

void Agent::ProcessInput(const std::string& input) {
    bool expected {false};
    if (!is_processing_.compare_exchange_strong(expected, true)) {
        return;
    }

    const CompletionRequest request {BuildSystemPrompt(), input};

    std::thread([this, request] {
        HandleOutput(llm_service_.Complete(request));
        is_processing_ = false;
    }).detach();
}

void Agent::HandleOutput(std::string output) {
    btree_.Build(nlohmann::json::parse(output));
    llm_output_.Set(std::move(output));
}

std::string Agent::BuildSystemPrompt() const {
    std::string prompt {"You are a drone mission planner. Output ONLY a single valid JSON behavior tree.\n"};

    prompt += "\nYour initial telemetry is: " + GetVehicleTelemetry() + "\n";

    prompt += "Available node types:\n";
    for (const auto& node : node_catalog_.GetNodes()) {
        prompt += node->GetPrompt() + "\n";
    }

    prompt +=
        "\nRules:\n"
        "  - sequence, fallback, parallel must have a non-empty \"children\" array\n"
        "  - parallel requires an integer \"success_threshold\" >= 1\n"
        "  - action nodes have exactly one intent key besides \"type\"\n"
        "  - Output raw JSON only. No markdown fences, no explanation.\n";

    return prompt;
}

void Agent::WalkOnTree(const Node* root) {
    if (root == nullptr) {
        return;
    }

    spdlog::info("Agent::WalkOnTree: Node type is '{}'.", typeid(root).name());

    for (const auto& child : root->GetChildrens()) {
        WalkOnTree(child.get());
    }
}
