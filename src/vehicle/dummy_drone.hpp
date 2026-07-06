#pragma once

#include "vehicle.hpp"

class DummyDrone : public Vehicle {
 public:
    void Arm() override;
    void Disarm() override;
    void GoTo(double latitude_deg, double longitude_deg, float absolute_altitude_m, float yaw_deg) override;
    void Init() override;
    void Kill() override;
    void Land() override;
    void Takeoff() override;

    Telemetry GetTelemetry() override;
};
