#pragma once

#include "vehicle.hpp"

class DummyDrone : public Vehicle {
 public:
    void Arm() override;
    void Disarm() override;
    void Init() override;
    void Kill() override;

    Telemetry GetTelemetry() override;
};
