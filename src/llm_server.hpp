#pragma once

#include <filesystem>

class LlmServer {
 public:
    static constexpr int kDefaultPort {8889};

    LlmServer(std::filesystem::path exe_dir,
              std::filesystem::path model_path,
              int port = kDefaultPort);
    ~LlmServer();

    int Port() const;

 private:
    void WaitUntilReady();

    pid_t pid_ {-1};
    int port_;
};
