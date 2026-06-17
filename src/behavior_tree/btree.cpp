#include "btree.hpp"

#include <memory>
#include <utility>

#include <spdlog/spdlog.h>

#include "behavior_tree/nodes/fallback_node.hpp"
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
        std::unique_ptr<Node> node;

        if (type == "sequence") {
            node = std::make_unique<SequenceNode>();
        } else if (type == "fallback") {
            node = std::make_unique<FallbackNode>();
        } else if (type == "parallel") {
            int threshold = json_node.at("success_threshold").get<int>();
            node = std::make_unique<ParallelNode>(threshold);
        } else if (type == "action") {

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
