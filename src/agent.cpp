#include "agent.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <thread>
#include <utility>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "behavior_tree/nodes/fallback_node.hpp"
#include "behavior_tree/nodes/move_nodes/move_node.hpp"
#include "behavior_tree/nodes/parallel_node.hpp"
#include "behavior_tree/nodes/sequence_node.hpp"
#include "behavior_tree/nodes/task_nodes/task_node.hpp"

import lifecycle;

namespace {

std::filesystem::path CreateRequestDirectory(const std::filesystem::path& root) {
    const auto timestamp {
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count()
    };
    const std::filesystem::path request_dir {root / std::to_string(timestamp)};
    std::filesystem::create_directories(request_dir);
    return request_dir;
}

void WriteText(const std::filesystem::path& path, const std::string& value) {
    std::ofstream output {path};
    if (!output) {
        throw std::runtime_error {"Could not write " + path.string()};
    }
    output << value << '\n';
}

}  // namespace

Agent::Agent(Vehicle& vehicle, LlmService& llm_service, RuntimeConfig config)
    : llm_service_ {llm_service},
      vehicle_ {vehicle},
      config_ {std::move(config)} {}

void Agent::Run() {
    while (lifecycle::is_alive_public) {
        {
            std::lock_guard lock {btree_mutex_};
            if (btree_.GetRoot()) {
                auto status {TickNode(btree_.GetRoot())};
                if (status == NodeStatus::Success) {
                    spdlog::info("Agent::Run: Success");
                    btree_.Destroy();
                } else if (status == NodeStatus::Failure) {
                    spdlog::info("Agent::Run: Failure");
                    btree_.Destroy();
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds {200});
    }
}

std::string Agent::GetVehicleTelemetry() const {
    const nlohmann::json telemetry = vehicle_.GetTelemetry();
    return telemetry.dump();
}

std::string Agent::GetOutput() {
    if (is_processing_) {
        return R"({"status":"processing"})";
    }
    const std::string output {llm_output_.Get()};
    return output.empty() ? R"({"status":"idle"})" : output;
}

void Agent::ArmVehicle() {
    vehicle_.Arm();
}

void Agent::DisarmVehicle() {
    vehicle_.Disarm();
}

void Agent::KillVehicle() {
    vehicle_.Kill();
}

void Agent::LandVehicle() {
    vehicle_.Land();
}

bool Agent::ProcessInput(const std::string& input) {
    bool expected {false};
    if (input.empty() || !is_processing_.compare_exchange_strong(expected, true)) {
        return false;
    }

    const Telemetry telemetry {vehicle_.GetTelemetry()};
    llm_output_.Set("");
    request_thread_ = std::jthread {
        [this, input, telemetry] {
            ProcessRequest(input, telemetry);
        }
    };
    return true;
}

void Agent::ReturnVehicle() {
    vehicle_.Rtl();
}

void Agent::ProcessRequest(std::string input, Telemetry telemetry) {
    try {
        const auto request_dir = CreateRequestDirectory(config_.inference_log_dir);
        WriteText(request_dir / "prompt.txt", input);

        const CompletionRequest request {
            BuildSystemPrompt(telemetry),
            input,
            RenderMapImage(config_, telemetry, request_dir)
        };
        std::string output {llm_service_.Complete(request)};
        if (output.empty()) {
            throw std::runtime_error {"Model returned no output"};
        }

        WriteText(request_dir / "raw_response.txt", output);
        HandleOutput(std::move(output), request_dir);
    } catch (const std::exception& error) {
        spdlog::error("Agent::ProcessRequest: {}", error.what());
        llm_output_.Set(nlohmann::json{{"error", error.what()}}.dump());
    }
    is_processing_ = false;
}

void Agent::HandleOutput(
    std::string output,
    const std::filesystem::path& request_dir
) {
    nlohmann::json json_tree = nlohmann::json::parse(output);
    BTree candidate;
    if (!candidate.Build(json_tree)) {
        throw std::runtime_error {"Model returned an invalid behavior tree"};
    }
    WriteText(request_dir / "btree.json", json_tree.dump(2));
    std::lock_guard lock {btree_mutex_};
    btree_ = std::move(candidate);
    llm_output_.Set(std::move(output));
}

std::string Agent::BuildSystemPrompt(const Telemetry& telemetry) const {
    std::string prompt {
        "You are a drone mission planner. Output ONLY a single valid JSON behavior tree.\n"
    };

    const nlohmann::json telemetry_json = telemetry;
    prompt += "\nYour initial telemetry is: " + telemetry_json.dump() + "\n";
    prompt +=
        "\nThe attached map is centered on the drone and uses WGS84 longitude/latitude "
        "axes. Contour labels are approximate ground elevation in meters ASL. Use map "
        "labels and terrain to resolve named destinations and reference altitudes.\n";

    prompt +=
        "\nAltitudes are meters above ground, not sea level. The reference defaults to "
        "home's elevation - omit \"reference_altitude_m\" unless the destination's ground "
        "differs (e.g. from a map), in which case set it to that elevation and let "
        "\"relative_altitude_m\" be the clearance above it; never add them yourself.\n"
        "go_to always needs \"relative_altitude_m\" - reuse the previous movement's value "
        "(or the 10m takeoff default) if the user didn't specify one.\n";

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

    if (auto task {dynamic_cast<TaskNode*>(node)}) {
        task->Execute({});
        return task->GetStatus();
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
