#pragma once

#include <atomic>
#include <string>

#include "agent.hpp"

class Server {
 public:
    explicit Server(Agent& agent, const int port = 8888);

    void Run();
    bool Started() const;

 private:
    std::string ProcessCommand(const std::string& command);

    Agent& agent_;
    const int port_;
    std::atomic<bool> started_ {false};
};
