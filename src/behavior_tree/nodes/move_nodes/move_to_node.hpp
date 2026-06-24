#pragma once

#include "move_node.hpp"

class MoveToNode : public MoveNode {
 public:
    MoveToNode(double latitude_deg, double longitude_deg, float relative_altitude_m);

    //std::vector<MissionItem> GetMissionItems() const override;

    NodeStatus GetStatus() const override;
    bool Validate() const override;
    std::string GetPrompt() const override;

 private:
    double latitude_deg_;
    double longitude_deg_;
    float relative_altitude_m_;
};
