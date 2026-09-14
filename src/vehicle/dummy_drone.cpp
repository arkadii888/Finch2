#include "dummy_drone.hpp"

#include <spdlog/spdlog.h>

namespace {
constexpr double kHomeLatitudeDeg {46.982'426};
constexpr double kHomeLongitudeDeg {7.431'551};
}

DummyDrone::DummyDrone()
    // Bern coverage used by the offline DEM / GeoPackage map.
    : telemetry_ {
          .latitude_deg = kHomeLatitudeDeg,
          .longitude_deg = kHomeLongitudeDeg,
          .absolute_altitude_m = 579.81f,
          .current_battery_a = 10.0f,
          .home_absolute_altitude_m = 579.81f,
          .remaining_percent = 100.0f,
          .voltage_v = 12.6f,
          .yaw_deg = 180.0f,
      } {}

void DummyDrone::Arm() {
    telemetry_.is_armed = true;
    spdlog::info("DummyDrone::Arm: Done.");
}

void DummyDrone::Disarm() {
    telemetry_.is_armed = false;
    spdlog::info("DummyDrone::Disarm: Done.");
}

void DummyDrone::GoTo(
    double latitude_deg,
    double longitude_deg,
    float absolute_altitude_m,
    float yaw_deg
) {
    telemetry_.latitude_deg = latitude_deg;
    telemetry_.longitude_deg = longitude_deg;
    telemetry_.absolute_altitude_m = absolute_altitude_m;
    telemetry_.relative_altitude_m =
        absolute_altitude_m - telemetry_.home_absolute_altitude_m;
    telemetry_.yaw_deg = yaw_deg;
    spdlog::info(
        "DummyDrone::GoTo: lat={} lon={} abs_alt={} yaw={}",
        latitude_deg,
        longitude_deg,
        absolute_altitude_m,
        yaw_deg
    );
}

void DummyDrone::Init() {
    spdlog::info("DummyDrone::Init: Drone is ready to arm.");
}

void DummyDrone::Kill() {
    telemetry_.is_armed = false;
    spdlog::info("DummyDrone::Kill: Done.");
}

void DummyDrone::Land() {
    telemetry_.is_armed = false;
    telemetry_.relative_altitude_m = 0.0f;
    telemetry_.absolute_altitude_m = telemetry_.home_absolute_altitude_m;
    spdlog::info("DummyDrone::Land: Done.");
}

void DummyDrone::Rtl() {
    telemetry_.latitude_deg = kHomeLatitudeDeg;
    telemetry_.longitude_deg = kHomeLongitudeDeg;
    telemetry_.is_armed = false;
    telemetry_.relative_altitude_m = 0.0f;
    telemetry_.absolute_altitude_m = telemetry_.home_absolute_altitude_m;
    spdlog::info("DummyDrone::Rtl: Done.");
}

void DummyDrone::Takeoff(float relative_altitude_m) {
    telemetry_.is_armed = true;
    telemetry_.relative_altitude_m = relative_altitude_m;
    telemetry_.absolute_altitude_m =
        telemetry_.home_absolute_altitude_m + relative_altitude_m;
    spdlog::info("DummyDrone::Takeoff: relative_altitude_m={}", relative_altitude_m);
}

Telemetry DummyDrone::GetTelemetry() {
    return telemetry_;
}
