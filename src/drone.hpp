#pragma once

#include <any>

class IDrone {
 public:
    virtual ~IDrone() = default;

    virtual void Init() = 0;
    virtual void LaunchMission(const std::any& mission_items) = 0;
    virtual void Kill() = 0;
    virtual void Arm() = 0;
    virtual void Disarm() = 0;

    virtual std::any GetTelemetry() = 0;
};
