#pragma once

#include <filesystem>
#include <optional>
#include <string>

struct CompletionRequest {
    std::string system_prompt;
    std::string user_prompt;
    std::optional<std::filesystem::path> image_path;
};

class LlmService {
 public:
    virtual ~LlmService() = default;

    virtual void Run() = 0;
    virtual void Stop() = 0;

    virtual std::string Complete(const CompletionRequest& request) = 0;
};
