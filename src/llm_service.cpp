#include "llm_service.hpp"

#include <stdexcept>
#include <utility>

LlmService::LlmService(LlmHttpBackend& backend) : backend_ {backend} {
    worker_ = std::thread {[this]() { WorkerLoop(); }};
}

LlmService::~LlmService() {
    {
        std::lock_guard<std::mutex> lock {mutex_};
        stop_ = true;
    }
    cv_.notify_one();
    if (worker_.joinable()) {
        worker_.join();
    }
}

// Single caller only. Concurrent Complete() calls share one pending_prompt_/result_ slot.
std::string LlmService::Complete(const std::string& prompt) {
    std::unique_lock<std::mutex> lock {mutex_};
    pending_prompt_ = prompt;
    result_.reset();
    cv_.notify_one();

    cv_.wait(lock, [this]() { return result_.has_value() || stop_; });

    if (stop_ && !result_.has_value()) {
        throw std::runtime_error {"LlmService::Complete: service stopped"};
    }

    std::string response {std::move(*result_)};
    result_.reset();
    return response;
}

void LlmService::WorkerLoop() {
    while (true) {
        std::string prompt;

        {
            std::unique_lock<std::mutex> lock {mutex_};
            cv_.wait(lock, [this]() { return stop_ || pending_prompt_.has_value(); });

            if (stop_ && !pending_prompt_.has_value()) {
                return;
            }

            prompt = std::move(*pending_prompt_);
            pending_prompt_.reset();
        }

        std::string response;
        try {
            response = backend_.Complete(prompt);
        } catch (const std::exception& e) {
            response = std::string {"Error: "} + e.what();
        }

        {
            std::lock_guard<std::mutex> lock {mutex_};
            result_ = std::move(response);
        }
        cv_.notify_one();
    }
}
