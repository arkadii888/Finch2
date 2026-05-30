#pragma once

#include <string>

#include "agent.hpp"

class Server {
 public:
    Server(Agent& agent, const int port = 8888);

    void Run();

 private:
    std::string ProcessCommand(const std::string& command);

    Agent& agent_;
    const int port_;
};
