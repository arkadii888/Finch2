#include "node_catalog.hpp"

#include <stdexcept>

#include "move_node.hpp"
#include "task_node.hpp"

std::vector<std::string> GetActionPrompts() {
    return {
        MoveTo::GetPrompt(),
        ComputeFibonacci::GetPrompt(),
    };
}

std::unique_ptr<Node> ParseActionNode(
    const std::string& key,
    const nlohmann::json& args,
    std::vector<MissionItem>& mission_items,
    std::vector<Move*>& moves)
{
    if (key == "move_to") {
        auto node = MoveTo::FromJson(args, mission_items);
        moves.push_back(static_cast<Move*>(node.get()));
        return node;
    }
    if (key == "compute_fibonacci") {
        return ComputeFibonacci::FromJson(args);
    }
    throw std::runtime_error {"unknown action: " + key};
}
