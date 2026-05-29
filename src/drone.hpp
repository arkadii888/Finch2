#pragma once

#include <vector>
#include <cmath>

struct Telemetry {
    double latitude_deg {0.0f};
    double longitude_deg {0.0f};
    float absolute_altitude_m {0.0f};
    float current_battery_a {0.0f};
    float relative_altitude_m {0.0f};
    float remaining_percent {0.0f};
    float voltage_v {0.0f};
    float yaw_deg {0.0f};
};

struct MissionItem {
    bool is_fly_through {false};
    double camera_photo_interval_s {1.0};
    double latitude_deg {NAN};
    double longitude_deg {NAN};
    float acceptance_radius_m {NAN};
    float camera_photo_distance_m {NAN};
    float gimbal_pitch_deg {NAN};
    float gimbal_yaw_deg {NAN};
    float loiter_time_s {NAN};
    float relative_altitude_m {NAN};
    float speed_m_s {NAN};
    float yaw_deg {NAN};
    int camera_action {0};
    int vehicle_action {0};
};

class IDrone {
 public:
    virtual ~IDrone() = default;

    virtual void Arm() = 0;
    virtual void Disarm() = 0;
    virtual void Init() = 0;
    virtual void Kill() = 0;
    virtual void LaunchMission(const std::vector<MissionItem>& mission_items) = 0;

    virtual Telemetry GetTelemetry() = 0;
};
