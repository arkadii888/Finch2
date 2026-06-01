#pragma once

#include <atomic>

#include "agent.hpp"

extern std::atomic<bool> global_running;

class Server {
 public:
    Server(Agent& agent);

    void Run();

 private:
    Agent& agent_;
};
