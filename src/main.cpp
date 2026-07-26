#include <thread>

#include <spdlog/spdlog.h>

#include "agent.hpp"
#include "api_server.hpp"
#include "llm_service/llama_service.hpp"
#include "logger.hpp"
#include "runtime_config.hpp"
#include "vehicle/dummy_drone.hpp"

import lifecycle;

int main() {
    lifecycle::Init();

    Logger logger;

    const RuntimeConfig config {RuntimeConfig::Make()};
    config.Validate();

    // Bern coverage used by the offline DEM / OSM tile cache.
    const Telemetry dummy_telemetry {
        .latitude_deg = 46.982'426,
        .longitude_deg = 7.431'551,
        .absolute_altitude_m = 579.81f,
        .current_battery_a = 10.0f,
        .home_absolute_altitude_m = 579.81f,
        .remaining_percent = 100.0f,
        .voltage_v = 12.6f,
        .yaw_deg = 180.0f,
    };
    DummyDrone vehicle {dummy_telemetry};
    vehicle.Init();

    LlamaService llama {config};
    llama.Run();

    {
        Agent agent {vehicle, llama, config};
        ApiServer server {agent};

        std::thread agent_thread {[&agent]() {
            agent.Run();
        }};
        std::thread server_thread {[&server]() {
            server.Run();
        }};

        agent_thread.join();
        server_thread.join();
    }

    llama.Stop();

    return 0;
}
