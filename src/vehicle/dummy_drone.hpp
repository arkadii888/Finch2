#pragma once

#include "vehicle.hpp"

class DummyDrone : public Vehicle {
 public:
    DummyDrone();

    void Arm() override;
    void Disarm() override;
    void GoTo(
        double latitude_deg,
        double longitude_deg,
        float absolute_altitude_m,
        float yaw_deg
    ) override;
    void Init() override;
    void Kill() override;
    void Land() override;
    void Rtl() override;
    void Takeoff(float relative_altitude_m) override;

    Telemetry GetTelemetry() override;

 private:
    Telemetry telemetry_;
    const double home_latitude_deg_ {46.989'705};
    const double home_longitude_deg_ {7.463'160};
    const float home_absolute_altitude_m_ {554.f};
};
