#include <thread>
#include <csignal>
#include <atomic>

#include "agent.hpp"
#include "dummy_drone.hpp"
#include "server.hpp"
#include "logger.hpp"

std::atomic<bool> global_running {true};

void signal_handler(int signum) {
    global_running = false;
}

int main() {
    Logger logger;

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    DummyDrone drone;
    drone.Init();

    Agent agent {drone};
    Server server {agent};

    std::thread agent_thread {[&agent](){
        agent.Run();
    }};

    std::thread server_thread {[&server](){
        server.Run();
    }};

    agent_thread.join();
    server_thread.join();

    return 0;
}
