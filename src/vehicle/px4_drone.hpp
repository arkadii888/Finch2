#pragma once

#include <memory>

#include <mavsdk.h>
#include <plugins/action/action.h>
#include <plugins/mission/mission.h>
#include <plugins/telemetry/telemetry.h>

#include "vehicle.hpp"

class Px4Drone : public Vehicle {
 public:
    void Arm() override;
    void Disarm() override;
    void GoTo(double latitude_deg, double longitude_deg, float absolute_altitude_m, float yaw_deg) override;
    void Init() override;
    void Kill() override;
    void Land() override;
    void Takeoff() override;

    Telemetry GetTelemetry() override;

 private:
    mavsdk::Mavsdk mavsdk_ {mavsdk::Mavsdk::Configuration {mavsdk::ComponentType::GroundStation}};

    std::shared_ptr<mavsdk::System> system_;
    std::unique_ptr<mavsdk::Action> action_;
    std::unique_ptr<mavsdk::Mission> mission_;
    std::unique_ptr<mavsdk::Telemetry> telemetry_;
};
