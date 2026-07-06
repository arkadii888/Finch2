#include <thread>

#include "agent.hpp"
#include "api_server.hpp"
#include "vehicle/dummy_drone.hpp"
#include "llm_service/llama_service.hpp"
#include "logger.hpp"

import lifecycle;

int main() {
    lifecycle::Init();

    Logger logger;

    DummyDrone vehicle;
    vehicle.Init();

    LlamaService llama;
    llama.Run();

    Agent agent {vehicle, llama};
    ApiServer server {agent};

    std::thread agent_thread {[&agent](){agent.Run();}};
    std::thread server_thread {[&server](){server.Run();}};

    agent_thread.join();
    server_thread.join();

    llama.Stop();

    return 0;
}
