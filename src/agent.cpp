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
#include "config/llm.hpp"

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

Agent::Agent(Vehicle& vehicle, LlmService& llm_service, LlmConfig config)
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
            BuildUserPrompt(telemetry, input),
            RenderMapImage(
                config_,
                telemetry.latitude_deg,
                telemetry.longitude_deg,
                request_dir
            )
        };
        std::string output {llm_service_.Complete(request)};
        if (output.empty()) {
            throw std::runtime_error {"Model returned no output"};
        }

        WriteText(request_dir / "raw_response.txt", output);
        HandleOutput(std::move(output),
            {telemetry.latitude_deg,
            telemetry.longitude_deg,
            config_.map_half_window_m},
            request_dir);
    } catch (const std::exception& error) {
        spdlog::error("Agent::ProcessRequest: {}", error.what());
        llm_output_.Set(nlohmann::json{{"error", error.what()}}.dump());
    }
    is_processing_ = false;
}

void Agent::HandleOutput(
    std::string output,
    const MapBounds& bounds,
    const std::filesystem::path& request_dir
) {
    nlohmann::json unprocessed_tree = nlohmann::json::parse(output);
    nlohmann::json processed_tree = ConvertPixelsToCoordinates(unprocessed_tree, bounds, config_.max_go_to);
    BTree candidate;
    if (!candidate.Build(processed_tree)) {
        throw std::runtime_error {"Model returned an invalid behavior tree"};
    }
    WriteText(request_dir / "btree.json", processed_tree.dump(2));
    std::lock_guard lock {btree_mutex_};
    btree_ = std::move(candidate);
    llm_output_.Set(processed_tree.dump());
}

std::string Agent::BuildUserPrompt(
    const Telemetry& telemetry,
    const std::string& mission
) const {
    const nlohmann::json telemetry_json {
        {"latitude_deg", telemetry.latitude_deg},
        {"longitude_deg", telemetry.longitude_deg},
        {"absolute_altitude_m", telemetry.absolute_altitude_m},
        {"home_absolute_altitude_m", telemetry.home_absolute_altitude_m},
        {"relative_altitude_m", telemetry.relative_altitude_m},
        {"yaw_deg", telemetry.yaw_deg},
        {"is_armed", telemetry.is_armed},
    };

    std::string prompt {
        "You are a drone mission planner. Output ONLY a single valid JSON behavior tree.\n"
        "Map image is centered on the drone.\n"
        "\nTelemetry: " + telemetry_json.dump() + "\n"
    };

    const std::string half_window {
        std::to_string(static_cast<int>(config_.map_half_window_m)) + " m"
    };
    prompt +=
        "\nMap: north-up geographic square filling the image; labels locate places; "
        "contour labels are ground ASL (coarse OK). Read named places from the map. "
        "go_to x,y are integers in [0,1000] on the map image: (0,0) is the top-left, "
        "(1000,1000) is the bottom-right, x right, y down. The drone / map center is "
        "about (500,500). Compass offsets in meters: 1 x-unit = 2*" + half_window
        + "/1000 m east, 1 y-unit = 2*" + half_window + "/1000 m south. "
        "Plain integers — never formulas, never latitude_deg/longitude_deg. "
        "Every go_to must stay inside [0,1000].\n";

    prompt +=
        "\nAltitudes: relative_altitude_m is AGL above the reference. Always include "
        "reference_altitude_m set it from contours or from home ground altitude."
        "(approx ASL OK). Always include relative_altitude_m on go_to — reuse the previous "
        "value (or 10m takeoff default) if unspecified. Never add reference+relative.\n"
        "\nWaypoints: Generate the minimum waypoint set required to execute the mission.\n"
        "Always include:\n"
        "- route start and destination;\n"
        "- required named targets;\n"
        "- relevant junctions, entrances, exits, and meaningful turns.\n"
        "Add intermediate waypoints only when adjacent required points are more than 200 m "
        "apart. Do not add nearly collinear or duplicate waypoints.\n"
        "Create at most " + std::to_string(config_.max_go_to) + " waypoints.\n";

    prompt +=
        "\nNodes:\n"
        "sequence: {\"type\":\"sequence\",\"children\":[...]}\n"
        "fallback: {\"type\":\"fallback\",\"children\":[...]}\n"
        "parallel: {\"type\":\"parallel\",\"success_threshold\":1,\"children\":[...]}\n"
        "go_to: {\"type\":\"action\",\"go_to\":{\"x\":<int>,\"y\":<int>,"
        "\"relative_altitude_m\":<f>,\"reference_altitude_m\":<f optional AMSL>,"
        "\"yaw_deg\":<f optional>}}\n"
        "land: {\"type\":\"action\",\"land\":{}}\n"
        "rtl: {\"type\":\"action\",\"rtl\":{}}\n"
        "takeoff: {\"type\":\"action\",\"takeoff\":{\"relative_altitude_m\":<f optional>}}\n"
        "\nRules: non-empty children; parallel success_threshold>=1; one intent per action; "
        "raw JSON only with numeric literals.\n"
        "\nMission: " + mission;

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
