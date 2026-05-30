#pragma once

#include "drone.hpp"

class DummyDrone : public Drone {
 public:
    void Arm() override;
    void Disarm() override;
    void Init() override;
    void Kill() override;
    void LaunchMission(const std::vector<MissionItem>& mission_items) override;

    Telemetry GetTelemetry() override;
};
