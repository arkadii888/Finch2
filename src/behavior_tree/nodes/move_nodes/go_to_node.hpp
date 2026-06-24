#pragma once

#include "move_node.hpp"

class GoToNode : public MoveNode {
 public:
    GoToNode(double latitude_deg, double longitude_deg, float absolute_altitude_m, float yaw_deg);

    void Execute() override;
    NodeStatus GetStatus() const override;
    bool Validate() const override;
    std::string GetPrompt() const override;

 private:
    double latitude_deg_;
    double longitude_deg_;
    float absolute_altitude_m_;
    float yaw_deg_;
};
