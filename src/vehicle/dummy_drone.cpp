#include "dummy_drone.hpp"

#include <spdlog/spdlog.h>

DummyDrone::DummyDrone()
    // Bern coverage used by the offline DEM / GeoPackage map.
    : telemetry_ {
          .latitude_deg = 46.982'426,
          .longitude_deg = 7.431'551,
          .absolute_altitude_m = 579.81f,
          .current_battery_a = 10.0f,
          .home_absolute_altitude_m = 579.81f,
          .remaining_percent = 100.0f,
          .voltage_v = 12.6f,
          .yaw_deg = 180.0f,
      } {}

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
