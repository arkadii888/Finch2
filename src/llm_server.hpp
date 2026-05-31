#pragma once

#include <filesystem>

class LlmServer {
 public:
    static constexpr int DEFAULT_PORT {8889};

    LlmServer(std::filesystem::path exe_dir,
              std::filesystem::path model_path,
              int port = DEFAULT_PORT);
    ~LlmServer();

    LlmServer(const LlmServer&) = delete;
    LlmServer& operator=(const LlmServer&) = delete;

    int Port() const;

 private:
    void WaitUntilReady();

    pid_t pid_ {-1};
    int port_;
};
