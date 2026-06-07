#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include <nlohmann/json.hpp>

#include "drone/drone.hpp"

struct Intent {
    std::string arg_description;
    std::function<std::vector<MissionItem>(const nlohmann::json&)> expand;
};

using IntentRegistry = std::unordered_map<std::string, Intent>;

inline const IntentRegistry& RegisteredIntents() {
    static const IntentRegistry registry = [] {
        IntentRegistry registry {};
        registry.emplace("move_to", Intent {
            .arg_description = R"({"lat": <degrees float>, "lon": <degrees float>})",
            .expand = [](const nlohmann::json& node) -> std::vector<MissionItem> {
                MissionItem item {};
                item.latitude_deg = node["move_to"]["lat"];
                item.longitude_deg = node["move_to"]["lon"];
                return {item};
            },
        });
        return registry;
    }();
    return registry;
}

inline std::string BuildSystemPrompt(const IntentRegistry& registry) {
    std::string prompt {
        "You are a drone mission planner. Output ONLY a single JSON behavior tree.\n\n"
        "Allowed node types:\n"
        "  sequence, parallel (requires \"success_threshold\"), fallback, action, condition\n\n"
        "Available actions (use as the key inside an \"action\" node):\n"
    };

    for (const auto& [name, intent] : registry) {
        prompt += "  " + name + ": " + intent.arg_description + "\n";
    }

    prompt +=
        "\nExample:\n"
        "{ \"type\": \"sequence\", \"children\": [\n"
        "    { \"type\": \"action\", \"move_to\": {\"lat\": 48.1, \"lon\": 11.6} }\n"
        "]}\n";

    return prompt;
}

inline const std::string& SystemPrompt() {
    static const std::string prompt {BuildSystemPrompt(RegisteredIntents())};
    return prompt;
}
