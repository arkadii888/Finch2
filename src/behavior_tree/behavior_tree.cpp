#include "behavior_tree/behavior_tree.hpp"

#include <nlohmann/json.hpp>

#include "intents/intent_catalog.hpp"

std::string BehaviorTree::ValidateNode(const nlohmann::json& node, const IntentCatalog& catalog) {
    if (!node.is_object()) {
        return "node must be a JSON object";
    }
    if (!node.contains("type") || !node["type"].is_string()) {
        return "node must have a string \"type\" field";
    }

    const std::string type {node["type"].get<std::string>()};

    if (type == "sequence" || type == "fallback" || type == "parallel") {
        if (type == "parallel") {
            if (!node.contains("success_threshold")
                    || !node["success_threshold"].is_number_integer()) {
                return "parallel node must have an integer \"success_threshold\" field";
            }
        }
        if (!node.contains("children") || !node["children"].is_array()) {
            return type + " node must have a \"children\" array";
        }
        for (const auto& child : node["children"]) {
            const std::string error {ValidateNode(child, catalog)};
            if (!error.empty()) {
                return error;
            }
        }
        return {};
    }

    if (type == "action") {
        std::string intent_name {};
        for (const auto& [key, _] : node.items()) {
            if (key == "type") {
                continue;
            }
            if (!intent_name.empty()) {
                return "action node must have exactly one intent key besides \"type\"";
            }
            intent_name = key;
        }
        if (intent_name.empty() || catalog.Find(intent_name) == nullptr) {
            return intent_name.empty()
                ? "action node must have exactly one intent key besides \"type\""
                : "unknown action intent: " + intent_name;
        }
        return {};
    }

    if (type == "condition") {
        if (!node.contains("check") || !node["check"].is_string()) {
            return "condition node must have a string \"check\" field";
        }
        return {};
    }

    return "unknown node type: " + type;
}
