#pragma once

#include "agent.hpp"

class Server {
 public:
    Server(Agent& agent);

    void Run();

 private:
    std::string ProcessCommand(const std::string& command);

    Agent& agent_;
};
