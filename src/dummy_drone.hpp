#pragma once

#include "drone.hpp"

class DummyDrone : public Drone {
 public:
    void Arm() override;
    void Disarm() override;
    void Init() override;
    void Kill() override;
    void LaunchMission() override;
    void UploadMission(const std::vector<MissionItem>& mission_items) override;

    std::pair<int, int> GetMissionProgress() override;

    Telemetry GetTelemetry() override;
};
