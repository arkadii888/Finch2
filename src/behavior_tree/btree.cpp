#include "btree.hpp"

#include <memory>
#include <utility>

#include <spdlog/spdlog.h>

#include "behavior_tree/nodes/fallback_node.hpp"
#include "behavior_tree/nodes/move_nodes/move_to_node.hpp"
#include "behavior_tree/nodes/parallel_node.hpp"
#include "behavior_tree/nodes/sequence_node.hpp"

void BTree::Build(const nlohmann::json& tree) {
    root_ = std::move(CreateNode(tree));
}

void BTree::Destroy() {

}

const Node& BTree::GetRoot() const {
    return *root_;
}

std::unique_ptr<Node> BTree::CreateNode(const nlohmann::json& json_node) {
    try {
        std::string type {json_node.at("type").get<std::string>()};
        std::unique_ptr<Node> node {nullptr};

        if (type == "sequence") {
            node = std::make_unique<SequenceNode>();
        } else if (type == "fallback") {
            node = std::make_unique<FallbackNode>();
        } else if (type == "parallel") {
            int threshold = json_node.at("success_threshold").get<int>();
            node = std::make_unique<ParallelNode>(threshold);
        } else if (type == "action") {
            std::string intent;
            nlohmann::json intent_arguments;

            for (const auto& [key, value] : json_node.items()) {
                if (key != "type") {
                    intent = key;
                    intent_arguments = value;
                    break;
                }
            }

            if (intent.empty()) {
                spdlog::error("BTree::CreateNode: Action node missing intent key.");
                return nullptr;
            }

            if (intent == "move_to") {
                double latitude_deg {intent_arguments.at("latitude_deg").get<double>()};
                double longitude_deg {intent_arguments.at("longitude_deg").get<double>()};
                float relative_altitude_m {intent_arguments.at("relative_altitude_m").get<float>()};
                node = std::make_unique<MoveToNode>(latitude_deg, longitude_deg, relative_altitude_m);
            }
        }

        if (node == nullptr) {
            spdlog::error("BTree::CreateNode: Failed to create node for type '{}'.", type);
            return nullptr;
        }

        if (type == "sequence" || type == "fallback" || type == "parallel") {
            if (!json_node.contains("children") || !json_node.at("children").is_array()) {
                spdlog::error("BTree::CreateNode: Node with type '{}' missing childrens.", type);
                return nullptr;
            }

            for (const auto& json_child : json_node.at("children")) {
                auto child_node = CreateNode(json_child);
                node->AddChild(std::move(child_node));
            }
        }

        if (!node->Validate()) {
            spdlog::error("BTree::CreateNode: Node with '{}' type is not valid.", type);
            return nullptr;
        }

        return node;
    } catch (const nlohmann::json::exception& e) {
        spdlog::error("BTree::CreateNode: Error: {}", e.what());
        return nullptr;
    } catch (const std::exception& e) {
        spdlog::error("BTree::CreateNode: Error: {}", e.what());
        return nullptr;
    }
};
