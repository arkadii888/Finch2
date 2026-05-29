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
    return {};
}
