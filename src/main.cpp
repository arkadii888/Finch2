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

    DummyDrone vehicle;
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
