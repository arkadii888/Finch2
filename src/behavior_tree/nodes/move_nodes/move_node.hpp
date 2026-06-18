#pragma once

#include <string>
#include <vector>

#include "behavior_tree/nodes/action_node.hpp"
#include "drone/drone.hpp"

class MoveNode : public ActionNode {
 public:
    virtual std::vector<MissionItem> GetMissionItems() const = 0;

    NodeStatus GetStatus() const override;
    bool Validate() const override;
    std::string GetPrompt() const override;
};
