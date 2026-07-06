#include "dummy_drone.hpp"

#include <spdlog/spdlog.h>

void DummyDrone::Arm() {
    spdlog::info("DummyDrone::Arm: Done.");
}

void DummyDrone::Disarm() {
    spdlog::info("DummyDrone::Disarm: Done.");
}

void DummyDrone::GoTo(double latitude_deg, double longitude_deg, float absolute_altitude_m, float yaw_deg) {
    spdlog::info("DummyDrone::GoTo: Done.");
}

void DummyDrone::Init() {
    spdlog::info("DummyDrone::Init: Drone is ready to arm.");
}

void DummyDrone::Kill() {
    spdlog::info("DummyDrone::Kill: Done.");
}

void DummyDrone::Land() {
    spdlog::info("DummyDrone::Land: Done.");
}

void DummyDrone::Takeoff() {
    spdlog::info("DummyDrone::Takeoff: Done.");
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
