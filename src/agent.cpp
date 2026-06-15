#include "agent.hpp"

#include <chrono>
#include <thread>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "behavior_tree/composite_nodes.hpp"

import lifecycle;

Agent::Agent(Drone& drone, LlmService& llm_service)
    : drone_ {drone}, llm_service_ {llm_service} {}

void Agent::Run() {
    while (lifecycle::is_alive_public) {
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

    const CompletionRequest request {
        .system_prompt = BuildSystemPrompt(),
        .user_prompt = input,
    };

    std::thread([this, request] {
        HandleOutput(llm_service_.Complete(request));
        is_processing_ = false;
    }).detach();
}

void Agent::HandleOutput(std::string output) {
    try {
        BTree btree {BTree::Parse(nlohmann::json::parse(output))};

        if (!btree.Validate()) {
            spdlog::error("Agent::HandleOutput: Behavior tree validation failed.");
            llm_output_.Set(std::move(output));
            return;
        }

        spdlog::info("Agent::HandleOutput: Tree valid. {} mission item(s).",
            btree.GetMissionItems().size());

        drone_.UploadMission(btree.GetMissionItems());
        drone_.LaunchMission();

        btree_ = std::make_unique<BTree>(std::move(btree));

    } catch (const std::exception& e) {
        spdlog::error("Agent::HandleOutput: Parse error: {}", e.what());
    }

    llm_output_.Set(std::move(output));
}

std::string Agent::BuildSystemPrompt() const {
    std::string prompt {
        "You are a drone mission planner. Output ONLY a single valid JSON behavior tree.\n\n"
        "Available node types:\n"
    };

    for (auto node : node_catalog_.GetNodes()) {
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
