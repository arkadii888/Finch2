#include "px4_drone.hpp"

#include <chrono>
#include <iostream>
#include <thread>

void Px4Drone::Init() {
    mavsdk_.add_any_connection("udpin://127.0.0.1:14540");

    while (mavsdk_.systems().empty()) {
        std::cout << "Px4Drone::Init: Drone is not detected yet..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "Px4Drone::Init: Drone is detected." << std::endl;

    system_ = mavsdk_.systems().at(0);
    action_ = std::make_unique<mavsdk::Action>(system_);
    mission_ = std::make_unique<mavsdk::Mission>(system_);
    telemetry_ = std::make_unique<mavsdk::Telemetry>(system_);

    while (!telemetry_->health_all_ok()) {
        std::cout << "Px4Drone::Init: Drone is not ready to arm yet..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "Px4Drone::Init: Drone is ready to arm." << std::endl;
}

void Px4Drone::LaunchMission(const std::vector<MissionItem>& mission_items) {

}

void Px4Drone::Kill() {

}

void Px4Drone::Arm() {

}

void Px4Drone::Disarm() {

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
