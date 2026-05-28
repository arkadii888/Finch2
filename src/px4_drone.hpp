#pragma once

#include <memory>
#include <any>

#include <mavsdk.h>
#include <plugins/action/action.h>
#include <plugins/telemetry/telemetry.h>
#include <plugins/mission/mission.h>

#include "drone.hpp"

class Px4Drone : public IDrone {
 public:
    void Init() override;
    void LaunchMission(const std::any& mission_items) override;
    void Kill() override;
    void Arm() override;
    void Disarm() override;

    std::any GetTelemetry() override;

 private:
    mavsdk::Mavsdk mavsdk_ {mavsdk::Mavsdk::Configuration {mavsdk::ComponentType::GroundStation}};
    std::shared_ptr<mavsdk::System> system_;
    std::unique_ptr<mavsdk::Action> action_;
    std::unique_ptr<mavsdk::Telemetry> telemetry_;
    std::unique_ptr<mavsdk::Mission> mission_;
};
