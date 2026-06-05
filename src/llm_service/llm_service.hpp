#pragma once

#include <string>

class LlmService {
 public:
    virtual ~LlmService() = default;

    virtual void Run() = 0;
    virtual void Stop() = 0;

    virtual std::string Complete(const std::string& prompt) = 0;
};
