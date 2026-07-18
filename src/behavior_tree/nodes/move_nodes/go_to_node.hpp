#pragma once

#include <optional>

#include "move_node.hpp"

import globals;

class GoToNode : public MoveNode {
 public:
    GoToNode(double latitude_deg, double longitude_deg, float absolute_altitude_m, std::optional<float> yaw_deg = std::nullopt);

    void Execute(std::any context) override;
    NodeStatus GetStatus() override;
    bool Validate() const override;
    std::string GetPrompt() const override;

 private:
    double latitude_deg_;
    double longitude_deg_;
    float absolute_altitude_m_;
    std::optional<float> yaw_deg_;
};
