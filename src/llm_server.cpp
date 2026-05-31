#include "llm_server.hpp"

#include <httplib.h>

#include <chrono>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <thread>

#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

namespace {

constexpr int kHealthPollMs {500};
constexpr int kHealthTimeoutSec {120};

void RequireExists(const std::filesystem::path& path, const char* label) {
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error {
            std::string {"LlmServer: "} + label + " not found at " + path.string()};
    }
}

bool ChildRunning(const pid_t pid) {
    int status {0};
    return pid > 0 && waitpid(pid, &status, WNOHANG) == 0;
}

void ThrowIfChildExited(const pid_t pid, const int port) {
    if (!ChildRunning(pid)) {
        throw std::runtime_error {
            "LlmServer: llama-server exited"
            " (is port " + std::to_string(port) + " already in use?)"};
    }
}

}  // namespace

LlmServer::LlmServer(std::filesystem::path exe_dir,
                     std::filesystem::path model_path,
                     const int port)
    : port_ {port} {
    const auto server_bin {std::filesystem::absolute(exe_dir / "llama-server")};
    const auto model_abs {std::filesystem::absolute(model_path)};
    RequireExists(server_bin, "llama-server");
    RequireExists(model_abs, "model");

    const std::string port_str {std::to_string(port_)};
    const std::string model_str {model_abs.string()};

    pid_ = fork();
    if (pid_ < 0) {
        throw std::runtime_error {"LlmServer: fork failed"};
    }

    if (pid_ == 0) {
        if (chdir(exe_dir.c_str()) != 0) {
            _exit(1);
        }
#if defined(__APPLE__)
        setenv("DYLD_LIBRARY_PATH", exe_dir.c_str(), 1);
#endif
        execl(server_bin.c_str(),
              "llama-server",
              "-m",
              model_str.c_str(),
              "--host",
              "127.0.0.1",
              "--port",
              port_str.c_str(),
              "-c",
              "4096",
              nullptr);
        _exit(1);
    }

    WaitUntilReady();
}

LlmServer::~LlmServer() {
    if (pid_ > 0) {
        kill(pid_, SIGTERM);
        waitpid(pid_, nullptr, 0);
        pid_ = -1;
    }
}

int LlmServer::Port() const {
    return port_;
}

void LlmServer::WaitUntilReady() {
    httplib::Client client {"127.0.0.1", port_};
    client.set_connection_timeout(5, 0);
    client.set_read_timeout(5, 0);

    const auto deadline {
        std::chrono::steady_clock::now() + std::chrono::seconds {kHealthTimeoutSec}};

    while (std::chrono::steady_clock::now() < deadline) {
        ThrowIfChildExited(pid_, port_);

        if (auto health {client.Get("/health")}; health && health->status == 200) {
            return;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds {kHealthPollMs});
    }

    ThrowIfChildExited(pid_, port_);
    throw std::runtime_error {"LlmServer: llama-server did not become ready in time"};
}
