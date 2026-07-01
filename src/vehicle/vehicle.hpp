#pragma once

#include <nlohmann/json.hpp>

struct Telemetry {
    double latitude_deg {0.0};
    double longitude_deg {0.0};
    float absolute_altitude_m {0.0f};
    float current_battery_a {0.0f};
    float relative_altitude_m {0.0f};
    float remaining_percent {0.0f};
    float voltage_v {0.0f};
    float yaw_deg {0.0f};
    bool is_armed {false};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Telemetry,
    latitude_deg,
    longitude_deg,
    absolute_altitude_m,
    current_battery_a,
    relative_altitude_m,
    remaining_percent,
    voltage_v,
    yaw_deg,
    is_armed
)

class Vehicle {
 public:
    virtual ~Vehicle() = default;

    virtual void Arm() = 0;
    virtual void Disarm() = 0;
    virtual void GoTo(double latitude_deg, double longitude_deg, float absolute_altitude_m, float yaw_deg) = 0;
    virtual void Init() = 0;
    virtual void Kill() = 0;
    virtual void Land() = 0;
    virtual void Takeoff() = 0;

    virtual Telemetry GetTelemetry() = 0;
};
