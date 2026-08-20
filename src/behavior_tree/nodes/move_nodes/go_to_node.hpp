#pragma once

#include <optional>

#include "move_node.hpp"

import globals;

class GoToNode : public MoveNode {
 public:
    GoToNode(double latitude_deg,
        double longitude_deg,
        float relative_altitude_m,
        std::optional<float> reference_altitude_m = std::nullopt,
        std::optional<float> yaw_deg = std::nullopt);

    void Execute(std::any context) override;
    NodeStatus GetStatus() override;
    bool Validate() const override;
    std::string GetPrompt() const override;

 private:
    float ResolveAbsoluteAltitudeM(const Telemetry& telemetry) const;

    double latitude_deg_;
    double longitude_deg_;
    float relative_altitude_m_;
    std::optional<float> reference_altitude_m_;
    std::optional<float> yaw_deg_;
};
