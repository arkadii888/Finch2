#pragma once

#include <string>

struct CompletionRequest {
    std::string system_prompt;
    std::string user_prompt;
};

class LlmService {
 public:
    virtual ~LlmService() = default;

    virtual void Run() = 0;
    virtual void Stop() = 0;

    // Thread-safe: may be called from a background thread.
    virtual std::string Complete(const CompletionRequest& request) = 0;
};
