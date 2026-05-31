#include <chrono>
#include <filesystem>
#include <iostream>
#include <thread>

#include "agent.hpp"
#include "dummy_drone.hpp"
#include "llm_http_backend.hpp"
#include "llm_server.hpp"
#include "llm_service.hpp"
#include "server.hpp"

namespace {

constexpr const char kModelRel[] {"models/gemma-4-E2B-it-Q4_K_M.gguf"};
constexpr const char kModelId[] {"gemma-4-E2B-it-Q4_K_M.gguf"};

}  // namespace

int main(int argc, char* argv[]) {
    (void)argc;

    const std::filesystem::path exe_dir {
        std::filesystem::weakly_canonical(argv[0]).parent_path()};
    const std::filesystem::path model_path {kModelRel};

    LlmServer llm_server {exe_dir, model_path};
    const int llm_port {llm_server.Port()};
    std::cout << "LlmServer: llama-server ready on port " << llm_port << std::endl;
    LlmHttpBackend backend {llm_port, kModelId};
    LlmService llm_service {backend};

    DummyDrone drone;
    Agent agent {drone, llm_service};
    Server server {agent};

    std::thread agent_thread {[&agent]() { agent.Run(); }};
    std::thread server_thread {[&server]() { server.Run(); }};

    for (int i {0}; i < 50 && !server.Started(); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds {100});
    }
    if (!server.Started()) {
        std::cerr << "Failed to start TCP server on port 8888. "
                  << "Stop other 'run' processes: pkill -f 'build-debug/bin/run'\n";
        return 1;
    }

    agent_thread.join();
    server_thread.join();

    return 0;
}
