#pragma once

#include <memory>

#include <nlohmann/json.hpp>

#include "nodes/node.hpp"

class BTree {
 public:
    void Build(const nlohmann::json& tree);
    void Destroy();

    const Node& GetRoot() const;

 private:
    std::unique_ptr<Node> CreateNode(const nlohmann::json& json_node);
    std::unique_ptr<Node> CreateActionNode(const nlohmann::json& json_action_node);

    std::unique_ptr<Node> root_;
};
