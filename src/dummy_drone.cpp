#include "dummy_drone.hpp"

#include <iostream>

void DummyDrone::Arm() {
    std::cout << "DummyDrone::Arm: Armed." << std::endl;
}

void DummyDrone::Disarm() {
    std::cout << "DummyDrone::Disarm: Disarmed." << std::endl;
}

void DummyDrone::Init() {
    std::cout << "DummyDrone::Init: Drone is ready to arm." << std::endl;
}

void DummyDrone::Kill() {
    std::cout << "DummyDrone::Kill: Killed." << std::endl;
}

void DummyDrone::LaunchMission(const std::vector<MissionItem>& mission_items) {
    std::cout << "DummyDrone::LaunchMission: Mission is launched." << std::endl;
}

Telemetry DummyDrone::GetTelemetry() {
    Telemetry t;
    t.latitude_deg = 45.467'1;
    t.longitude_deg = -73.757'8;
    t.absolute_altitude_m = 32.193f;
    t.current_battery_a = 10.0f;
    t.relative_altitude_m = 0.000f;
    t.remaining_percent = 100.0f;
    t.voltage_v = 12.6f;
    t.yaw_deg = 180.0f;
    return t;
}
