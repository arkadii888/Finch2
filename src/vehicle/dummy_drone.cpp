#include "dummy_drone.hpp"

#include <spdlog/spdlog.h>

DummyDrone::DummyDrone() {
    telemetry_.latitude_deg = home_latitude_deg_;
    telemetry_.longitude_deg = home_longitude_deg_;
    telemetry_.absolute_altitude_m = home_absolute_altitude_m_;
    telemetry_.current_battery_a = 10.0f;
    telemetry_.home_absolute_altitude_m = home_absolute_altitude_m_;
    telemetry_.remaining_percent = 100.0f;
    telemetry_.voltage_v = 12.6f;
    telemetry_.yaw_deg = 180.0f;
}

void DummyDrone::Arm() {
    telemetry_.is_armed = true;
    spdlog::info("DummyDrone::Arm: Done.");
}

void DummyDrone::Disarm() {
    telemetry_.is_armed = false;
    spdlog::info("DummyDrone::Disarm: Done.");
}

void DummyDrone::GoTo(double latitude_deg, double longitude_deg, float absolute_altitude_m, float yaw_deg) {
    telemetry_.latitude_deg = latitude_deg;
    telemetry_.longitude_deg = longitude_deg;
    telemetry_.absolute_altitude_m = absolute_altitude_m;
    telemetry_.relative_altitude_m = absolute_altitude_m - telemetry_.home_absolute_altitude_m;
    telemetry_.yaw_deg = yaw_deg;

    spdlog::info(
        "DummyDrone::GoTo: lat = {} lon = {} abs_alt = {} yaw = {}",
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
    telemetry_.latitude_deg = home_latitude_deg_;
    telemetry_.longitude_deg = home_longitude_deg_;
    telemetry_.is_armed = false;
    telemetry_.relative_altitude_m = 0.0f;
    telemetry_.absolute_altitude_m = telemetry_.home_absolute_altitude_m;
    spdlog::info("DummyDrone::Rtl: Done.");
}

void DummyDrone::Takeoff(float relative_altitude_m) {
    telemetry_.is_armed = true;
    telemetry_.relative_altitude_m = relative_altitude_m;
    telemetry_.absolute_altitude_m = telemetry_.home_absolute_altitude_m + relative_altitude_m;
    spdlog::info("DummyDrone::Takeoff: relative_altitude_m = {}", relative_altitude_m);
}

Telemetry DummyDrone::GetTelemetry() {
    return telemetry_;
}
