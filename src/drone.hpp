#pragma once

#include <vector>
#include <cmath>
#include <mutex>

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
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Telemetry,
    latitude_deg,
    longitude_deg,
    absolute_altitude_m,
    current_battery_a,
    relative_altitude_m,
    remaining_percent,
    voltage_v,
    yaw_deg
)

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

enum class MissionStatus {
    None,
    Started,
    Finished,
    Failed
};

struct MissionProgress {
    int total_items {0};
    int current_item {0};
    MissionStatus status {MissionStatus::None};
};

class MissionProgressThreadSafe {
 public:
    MissionProgress Get () {
        std::lock_guard lock {m_};
        return mission_progress_;
    };

    void Set(int total, int current, MissionStatus status) {
        std::lock_guard lock {m_};
        mission_progress_.total_items = total;
        mission_progress_.current_item = current;
        mission_progress_.status = status;
    }

 private:
    MissionProgress mission_progress_;
    std::mutex m_;
};

class Drone {
 public:
    virtual ~Drone() = default;

    virtual void Arm() = 0;
    virtual void Disarm() = 0;
    virtual void Init() = 0;
    virtual void Kill() = 0;
    virtual void LaunchMission() = 0;
    virtual void UploadMission(const std::vector<MissionItem>& mission_items) = 0;

    virtual MissionProgress GetMissionProgress() = 0;

    virtual Telemetry GetTelemetry() = 0;
};
