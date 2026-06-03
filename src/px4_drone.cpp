#include "px4_drone.hpp"
#include "drone.hpp"

#include <chrono>
#include <iostream>
#include <thread>

void Px4Drone::Init() {
    mavsdk_.add_any_connection("udpin://127.0.0.1:14540");

    while (mavsdk_.systems().empty()) {
        std::cout << "Px4Drone::Init: Drone is not detected yet..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds {1});
    }
    std::cout << "Px4Drone::Init: Drone is detected." << std::endl;

    system_ = mavsdk_.systems().at(0);
    action_ = std::make_unique<mavsdk::Action>(system_);
    mission_ = std::make_unique<mavsdk::Mission>(system_);
    telemetry_ = std::make_unique<mavsdk::Telemetry>(system_);

    while (!telemetry_->health_all_ok()) {
        std::cout << "Px4Drone::Init: Drone is not ready to arm yet..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds {1});
    }
    std::cout << "Px4Drone::Init: Drone is ready to arm." << std::endl;
}

void Px4Drone::LaunchMission() {
    while (!telemetry_->armed()) {
        std::this_thread::sleep_for(std::chrono::milliseconds {500});
    }

    if(mission_->start_mission() != mavsdk::Mission::Result::Success) {
        std::cout << "Vehicle::LaunchMission: Mission launch failed." << std::endl;
    }

    std::cout << "Vehicle::LaunchMission: Mission launched." << std::endl;
}

void Px4Drone::UploadMission(const std::vector<MissionItem>& mission_items) {
    if(mission_->pause_mission() != mavsdk::Mission::Result::Success) {
        std::cout << "Px4Drone::UploadMission: Mission pause failed." << std::endl;
        return;
    }

    if(mission_->clear_mission() != mavsdk::Mission::Result::Success) {
        std::cout << "Px4Drone::UploadMission: Mission clear failed." << std::endl;
        return;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds {500});

    std::size_t items_count {mission_items.size()};
    std::vector<mavsdk::Mission::MissionItem> final_items(items_count);

    for(int i {0}; i < items_count; ++i) {
        final_items[i].is_fly_through = mission_items[i].is_fly_through;
        final_items[i].camera_photo_interval_s = mission_items[i].camera_photo_interval_s;
        final_items[i].latitude_deg = mission_items[i].latitude_deg;
        final_items[i].longitude_deg = mission_items[i].longitude_deg;
        final_items[i].acceptance_radius_m = mission_items[i].acceptance_radius_m;
        final_items[i].camera_photo_distance_m = mission_items[i].camera_photo_distance_m;
        final_items[i].gimbal_pitch_deg = mission_items[i].gimbal_pitch_deg;
        final_items[i].gimbal_yaw_deg = mission_items[i].gimbal_yaw_deg;
        final_items[i].loiter_time_s = mission_items[i].loiter_time_s;
        final_items[i].relative_altitude_m = mission_items[i].relative_altitude_m;
        final_items[i].speed_m_s = mission_items[i].speed_m_s;
        final_items[i].yaw_deg = mission_items[i].yaw_deg;
        final_items[i].camera_action = static_cast<mavsdk::Mission::MissionItem::CameraAction>(
            mission_items[i].camera_action);
        final_items[i].vehicle_action = static_cast<mavsdk::Mission::MissionItem::VehicleAction>(
            mission_items[i].vehicle_action);
    }

    mavsdk::Mission::MissionPlan plan {};
    plan.mission_items = final_items;

    if(mission_->upload_mission(plan) != mavsdk::Mission::Result::Success) {
        std::cout << "Px4Drone::UploadMission: Mission upload failed." << std::endl;
        return;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds {500});

    mission_progress_.Set(items_count, 0, MissionStatus::Started);

    std::cout << "Px4Drone::UploadMission: Mission uploaded." << std::endl;
}

void Px4Drone::Kill() {
    if(action_->kill() != mavsdk::Action::Result::Success) {
        std::cout << "Px4Drone::Kill: Failed." << std::endl;
        return;
    }

    std::cout << "Px4Drone::Kill: Killed." << std::endl;
}

void Px4Drone::Arm() {
    if(action_->arm() != mavsdk::Action::Result::Success) {
        std::cout << "Px4Drone::Arm: Failed." << std::endl;
        return;
    }

    std::cout << "Px4Drone::Arm: Armed." << std::endl;
}

void Px4Drone::Disarm() {
    if(action_->disarm() != mavsdk::Action::Result::Success) {
        std::cout << "Px4Drone::Disarm: Failed." << std::endl;
        return;
    }

    std::cout << "Px4Drone::Arm: Disarmed." << std::endl;
}

MissionProgress Px4Drone::GetMissionProgress() {
    int current_item = mission_->mission_progress().current;

    auto progress = mission_progress_.Get();
    progress.current_item = current_item;

    if(progress.current_item == progress.total_items) {
        progress.status = MissionStatus::Finished;
    }

    mission_progress_.Set(progress.total_items, progress.current_item, progress.status);

    return progress;
}

Telemetry Px4Drone::GetTelemetry() {
    Telemetry t;

    auto position {telemetry_->position()};
    auto battery {telemetry_->battery()};
    auto euler {telemetry_->attitude_euler()};

    t.latitude_deg = position.latitude_deg;
    t.longitude_deg = position.longitude_deg;
    t.absolute_altitude_m = position.absolute_altitude_m;
    t.relative_altitude_m = position.relative_altitude_m;

    t.voltage_v = battery.voltage_v;
    t.current_battery_a = battery.current_battery_a;
    t.remaining_percent = battery.remaining_percent;

    t.yaw_deg = euler.yaw_deg;

    return t;
}
