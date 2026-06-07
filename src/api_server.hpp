#pragma once

#include <atomic>

#include "agent.hpp"

class ApiServer {
 public:
    ApiServer(Agent& agent);

    void Run();

 private:
    Agent& agent_;
};
