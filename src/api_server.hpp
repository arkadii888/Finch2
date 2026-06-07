#pragma once

#include <atomic>

#include "agent.hpp"

import globals;

extern std::atomic<bool> global_running;

class ApiServer {
 public:
    ApiServer(Agent& agent);

    void Run();

 private:
    Agent& agent_;
};
