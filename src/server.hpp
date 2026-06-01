#pragma once

#include <string>
#include <atomic>

#include "agent.hpp"

extern std::atomic<bool> global_running;

class Server {
 public:
    Server(Agent& agent, const unsigned short port = 8888);

    void Run();

 private:
    std::string ProcessCommand(const std::string& command);

    Agent& agent_;
    const unsigned short port_;
};
