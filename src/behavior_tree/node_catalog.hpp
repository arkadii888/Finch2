#pragma once

#include <memory>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "drone/drone.hpp"
#include "move_node.hpp"
#include "node.hpp"

// Returns one prompt string per registered action node type.
// Add a new action here when introducing a new Move or Task subclass.
std::vector<std::string> GetActionPrompts();

// Dispatches a JSON action node to the appropriate FromJson factory.
// Throws std::runtime_error for unknown action keys.
std::unique_ptr<Node> ParseActionNode(
    const std::string& key,
    const nlohmann::json& args,
    std::vector<MissionItem>& mission_items,
    std::vector<Move*>& moves);
