#include "px4_drone.hpp"

#include <chrono>
#include <thread>

#include <spdlog/spdlog.h>

void Px4Drone::Arm() {
    if (action_->arm() != mavsdk::Action::Result::Success) {
        spdlog::error("Px4Drone::Arm: Failed.");
        return;
    }

    spdlog::info("Px4Drone::Arm: Done.");
}

void Px4Drone::Disarm() {
    if (action_->disarm() != mavsdk::Action::Result::Success) {
        spdlog::error("Px4Drone::Disarm: Failed.");
        return;
    }

    spdlog::info("Px4Drone::Disarm: Done.");
}

void Px4Drone::GoTo(double latitude_deg, double longitude_deg, float absolute_altitude_m, float yaw_deg) {
    if (action_->goto_location(latitude_deg, longitude_deg, absolute_altitude_m, yaw_deg) != mavsdk::Action::Result::Success) {
        spdlog::error("Px4Drone::GoTo: Failed.");
        return;
    }

    spdlog::info("Px4Drone::GoTo: Done.");
}

void Px4Drone::Init() {
    mavsdk_.add_any_connection("udpin://127.0.0.1:14540");

    while (mavsdk_.systems().empty()) {
        spdlog::info("Px4Drone::Init: Not detected yet..");
        std::this_thread::sleep_for(std::chrono::seconds {1});
    }
    spdlog::info("Px4Drone::Init: Detected.");

    system_ = mavsdk_.systems().at(0);
    action_ = std::make_unique<mavsdk::Action>(system_);
    mission_ = std::make_unique<mavsdk::Mission>(system_);
    telemetry_ = std::make_unique<mavsdk::Telemetry>(system_);

    while (!telemetry_->health_all_ok()) {
        spdlog::info("Px4Drone::Init: Not ready to arm yet...");
        std::this_thread::sleep_for(std::chrono::seconds {1});
    }
    spdlog::info("Px4Drone::Init: Ready to arm.");
}

void Px4Drone::Kill() {
    if (action_->kill() != mavsdk::Action::Result::Success) {
        spdlog::error("Px4Drone::Kill: Failed.");
        return;
    }

    spdlog::info("Px4Drone::Kill: Done.");
}

void Px4Drone::Land() {
    if (action_->land() != mavsdk::Action::Result::Success) {
        spdlog::error("Px4Drone::Land: Failed.");
        return;
    }

    spdlog::info("Px4Drone::Land: Done.");
}

void Px4Drone::Takeoff() {
    while (!telemetry_->armed()) {
        spdlog::info("Px4Drone::Takeoff: Waiting for arm command...");
        std::this_thread::sleep_for(std::chrono::milliseconds {500});
    }

    if (action_->takeoff() != mavsdk::Action::Result::Success) {
        spdlog::error("Px4Drone::Takeoff: Failed.");
        return;
    }

    spdlog::info("Px4Drone::Takeoff: Done.");
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
