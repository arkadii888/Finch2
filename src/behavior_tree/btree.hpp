#pragma once

#include <memory>
#include <vector>

#include <nlohmann/json.hpp>

#include "drone/drone.hpp"
#include "move_node.hpp"
#include "node.hpp"

class BTree {
 public:
    static BTree Parse(const nlohmann::json& json);

    bool Validate() const;

    NodeStatus GetStatus(int current_wp, int total_wp) const;

    const std::vector<MissionItem>& GetMissionItems() const;
    const Node& GetRoot() const;

 private:
    Btree() = default;
    std::vector<MissionItem> mission_items_;
    std::vector<Move*> moves_;
    std::unique_ptr<Node> root_;
};
