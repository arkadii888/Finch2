#pragma once

#include <memory>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "drone/drone.hpp"
#include "move_node.hpp"
#include "node.hpp"

std::vector<std::string> GetActionPrompts();

std::unique_ptr<Node> ParseActionNode(
    const std::string& key,
    const nlohmann::json& args,
    std::vector<MissionItem>& mission_items,
    std::vector<Move*>& moves);
