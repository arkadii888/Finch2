#include "dummy_drone.hpp"

#include <spdlog/spdlog.h>

DummyDrone::DummyDrone(Telemetry telemetry) : telemetry_ {telemetry} {}

void DummyDrone::Arm() {
    spdlog::info("DummyDrone::Arm: Done.");
}

void DummyDrone::Disarm() {
    spdlog::info("DummyDrone::Disarm: Done.");
}

void DummyDrone::GoTo(
    double latitude_deg,
    double longitude_deg,
    float absolute_altitude_m,
    float yaw_deg
) {
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

void DummyDrone::Rtl() {
    spdlog::info("DummyDrone::Rtl: Done.");
}

void DummyDrone::Takeoff(float relative_altitude_m) {
    spdlog::info("DummyDrone::Takeoff: Done.");
}

Telemetry DummyDrone::GetTelemetry() {
    return telemetry_;
}
