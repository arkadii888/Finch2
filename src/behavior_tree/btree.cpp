#include "btree.hpp"

#include <stdexcept>
#include <string>

#include <nlohmann/json.hpp>

#include "composite_nodes.hpp"
#include "move_node.hpp"
#include "task_node.hpp"

static std::unique_ptr<Node> ParseNode(
    const nlohmann::json& j,
    std::vector<MissionItem>& mission_items,
    std::vector<Move*>& moves);

static std::unique_ptr<Node> ParseComposite(
    const nlohmann::json& j,
    const std::string& type,
    std::vector<MissionItem>& mission_items,
    std::vector<Move*>& moves)
{
    if (!j.contains("children") || !j["children"].is_array()) {
        throw std::runtime_error {type + " node must have a \"children\" array"};
    }

    std::unique_ptr<Node> node;

    if (type == "sequence") {
        node = std::make_unique<SequenceNode>();
    } else if (type == "fallback") {
        node = std::make_unique<FallbackNode>();
    } else {
        // parallel
        if (!j.contains("success_threshold")
                || !j["success_threshold"].is_number_integer()) {
            throw std::runtime_error {
                "parallel node must have an integer \"success_threshold\" field"};
        }
        node = std::make_unique<ParallelNode>(
            j["success_threshold"].get<int>());
    }

    for (const auto& child : j["children"]) {
        node->AddChild(ParseNode(child, mission_items, moves));
    }
    return node;
}

static std::unique_ptr<Node> ParseAction(
    const nlohmann::json& j,
    std::vector<MissionItem>& mission_items,
    std::vector<Move*>& moves)
{
    std::string intent_key;
    for (const auto& [key, val] : j.items()) {
        if (key == "type") continue;
        if (!intent_key.empty()) {
            throw std::runtime_error {
                "action node must have exactly one intent key besides \"type\""};
        }
        intent_key = key;
    }

    if (intent_key.empty()) {
        throw std::runtime_error {
            "action node must have exactly one intent key besides \"type\""};
    }

    if (auto* factory = MoveRegistry::Find(intent_key)) {
        auto node = (*factory)(j[intent_key], mission_items);
        moves.push_back(static_cast<Move*>(node.get()));
        return node;
    }

    if (auto* factory = TaskRegistry::Find(intent_key)) {
        return (*factory)(j[intent_key]);
    }

    throw std::runtime_error {"unknown action: " + intent_key};
}

static std::unique_ptr<Node> ParseNode(
    const nlohmann::json& j,
    std::vector<MissionItem>& mission_items,
    std::vector<Move*>& moves)
{
    if (!j.is_object()) {
        throw std::runtime_error {"node must be a JSON object"};
    }
    if (!j.contains("type") || !j["type"].is_string()) {
        throw std::runtime_error {"node must have a string \"type\" field"};
    }

    const std::string type {j["type"].get<std::string>()};

    if (type == "sequence" || type == "fallback" || type == "parallel") {
        return ParseComposite(j, type, mission_items, moves);
    }
    if (type == "action") {
        return ParseAction(j, mission_items, moves);
    }

    throw std::runtime_error {"unknown node type: " + type};
}

BTree BTree::Parse(const nlohmann::json& json) {
    BTree btree;
    btree.root_ = ParseNode(json, btree.mission_items_, btree.moves_);
    return btree;
}

bool BTree::Validate() const {
    if (!root_) return false;
    return root_->Validate();
}

NodeStatus BTree::GetStatus(int current_wp, int total_wp) const {
    for (Move* move : moves_) {
        move->UpdateProgress(current_wp);
    }
    return root_->GetStatus();
}

const std::vector<MissionItem>& BTree::GetMissionItems() const {
    return mission_items_;
}

const Node& BTree::GetRoot() const {
    return *root_;
}
