#pragma once

#include <atomic>

#include "agent.hpp"

extern std::atomic<bool> global_running;

class Server {
 public:
    Server(Agent& agent, const int port = 8888);

    void Run();

 private:
    Agent& agent_;
    const int port_;
};
