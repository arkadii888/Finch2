#include <thread>

#include "agent.hpp"
#include "dummy_drone.hpp"
#include "server.hpp"

int main() {
    DummyDrone drone;
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
