#include "agent.hpp"

#include <chrono>
#include <thread>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "behavior_tree/behavior_tree.hpp"

Agent::Agent(Drone& drone, LlmService& llm_service)
    : drone_ {drone}, llm_service_ {llm_service}, intent_catalog_ {IntentCatalog::MakeDefault()} {}

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
        const std::string error {
            BehaviorTree::ValidateNode(nlohmann::json::parse(output), intent_catalog_)};
        if (error.empty()) {
            spdlog::info("Agent::HandleOutput: Behavior tree validation passed.");
        } else {
            spdlog::error("Agent::HandleOutput: Behavior tree validation failed: {}", error);
        }
    } catch (const std::exception& e) {
        spdlog::error("Agent::HandleOutput: Failed to parse LLM output as JSON: {}", e.what());
    }

    llm_output_.Set(std::move(output));
}

std::string Agent::BuildSystemPrompt() const {
    std::string prompt {
        "You are a drone mission planner. Output ONLY a single JSON behavior tree.\n\n"
        "Allowed node types:\n"
        "  sequence, parallel (requires \"success_threshold\"), fallback, action, condition\n\n"
        "Available actions (use as the key inside an \"action\" node):\n"
    };

    for (const auto& [name, intent] : intent_catalog_.GetIntents()) {
        prompt += "  " + name + ": " + intent.arg_description + "\n";
    }

    prompt +=
        "\nExample:\n"
        "{ \"type\": \"sequence\", \"children\": [\n"
        "    { \"type\": \"action\", \"move_to\": {\"lat\": 48.1, \"lon\": 11.6} }\n"
        "]}\n";

    return prompt;
}
