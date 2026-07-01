#include "agent.hpp"

#include <chrono>
#include <thread>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "behavior_tree/nodes/fallback_node.hpp"
#include "behavior_tree/nodes/parallel_node.hpp"
#include "behavior_tree/nodes/sequence_node.hpp"
#include "behavior_tree/nodes/move_nodes/move_node.hpp"

import lifecycle;

Agent::Agent(Vehicle& vehicle, LlmService& llm_service)
    : vehicle_ {vehicle}, llm_service_ {llm_service} {}

void Agent::Run() {
    while (lifecycle::is_alive_public) {
        if (btree_.GetRoot()) { // TODO: make thread safe
            auto status {TickNode(btree_.GetRoot())};
            if (status == NodeStatus::Success) {
                spdlog::info("Agent::Run: Success");
                btree_.Destroy();
            } else if (status == NodeStatus::Failure) {
                spdlog::info("Agent::Run: Failure");
                btree_.Destroy();
            }
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

NodeStatus Agent::TickNode(Node* node) {
    if (node == nullptr) {
        return NodeStatus::Failure;
    }

    if (auto move {dynamic_cast<MoveNode*>(node)}) {
        if (!move->IsExecuted()) {
            move->Execute(&vehicle_);
        }
        return move->GetStatus();
    }

    if (auto sequence {dynamic_cast<SequenceNode*>(node)}) {
        for (auto& child : sequence->GetChildrens()) {
            auto status {TickNode(child.get())};
            if (status == NodeStatus::Failure) {
                return status;
            }
            if (status == NodeStatus::Running) {
                return status;
            }
        }
        return NodeStatus::Success;
    }

    if (auto fallback {dynamic_cast<FallbackNode*>(node)}) {
        for (auto& child : fallback->GetChildrens()) {
            auto status {TickNode(child.get())};
            if (status == NodeStatus::Success) {
                return status;
            }
            if (status == NodeStatus::Running) {
                return status;
            }
        }
        return NodeStatus::Failure;
    }

    if (auto parallel {dynamic_cast<ParallelNode*>(node)}) {
        int success_count {0};
        int failure_count {0};
        int success_threshold {parallel->GetSuccessThreshold()};

        auto& childrens {parallel->GetChildrens()};
        int childrens_count {static_cast<int>(childrens.size())};

        for (auto& child : childrens) {
            auto status {TickNode(child.get())};
            if (status == NodeStatus::Success) {
                ++success_count;
            }
            if (status == NodeStatus::Failure) {
                ++failure_count;
            }
        }

        if (success_count >= success_threshold) {
            return NodeStatus::Success;
        }
        if (failure_count > childrens_count - success_threshold) {
            return NodeStatus::Failure;
        }
        return NodeStatus::Running;
    }

    return NodeStatus::Success;
}
