#pragma once

#include "move_node.hpp"

import globals;

class TakeoffNode : public MoveNode {
 public:
    explicit TakeoffNode(float relative_altitude_m = globals::drone_takeoff_altitude_m);

    void Execute(std::any context) override;
    NodeStatus GetStatus() override;
    bool Validate() const override;
    std::string GetPrompt() const override;

 private:
    float relative_altitude_m_;
};
