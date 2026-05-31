#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <string>
#include <thread>

#include "llm_http_backend.hpp"

class LlmService {
 public:
    explicit LlmService(LlmHttpBackend& backend);
    ~LlmService();

    std::string Complete(const std::string& prompt);

 private:
    void WorkerLoop();

    LlmHttpBackend& backend_;
    std::thread worker_;
    std::mutex mutex_;
    std::condition_variable cv_;

    std::optional<std::string> pending_prompt_;
    std::optional<std::string> result_;
    bool stop_ {false};
};
