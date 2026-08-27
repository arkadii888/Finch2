#include "map_image_service.hpp"

#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
#include <chrono>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

std::filesystem::path RenderMapImage(
    const RuntimeConfig& config,
    const double latitude_deg,
    const double longitude_deg,
    const std::filesystem::path& request_dir
) {
    std::filesystem::create_directories(request_dir);
    const std::filesystem::path image_path {request_dir / "map.png"};
    const std::filesystem::path log_path {request_dir / "renderer.log"};

    std::vector<std::string> args {
        config.python_path.string(),
        config.renderer_path.string(),
        "--lat",
        std::to_string(latitude_deg),
        "--lon",
        std::to_string(longitude_deg),
        "--dem",
        config.dem_path.string(),
        "--map",
        config.map_path.string(),
        "--output",
        image_path.string(),
        "--half-window-m",
        std::to_string(config.map_half_window_m),
        "--zoom",
        std::to_string(config.map_zoom),
    };

    const pid_t pid {fork()};
    if (pid < 0) {
        throw std::runtime_error {"Could not fork map renderer"};
    }

    if (pid == 0) {
        const int log_fd {
            open(log_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644)
        };
        if (log_fd >= 0) {
            dup2(log_fd, STDOUT_FILENO);
            dup2(log_fd, STDERR_FILENO);
            close(log_fd);
        }
        std::vector<char*> argv;
        argv.reserve(args.size() + 1);
        for (auto& arg : args) {
            argv.push_back(arg.data());
        }
        argv.push_back(nullptr);
        execv(config.python_path.c_str(), argv.data());
        perror("Map renderer exec failed");
        _exit(1);
    }

    const auto deadline {
        std::chrono::steady_clock::now()
        + std::chrono::seconds {config.renderer_timeout_s}
    };
    int status {};
    bool finished {false};
    while (std::chrono::steady_clock::now() < deadline) {
        const pid_t result {waitpid(pid, &status, WNOHANG)};
        if (result == pid) {
            finished = true;
            break;
        }
        if (result < 0 && errno != EINTR) {
            throw std::runtime_error {"waitpid failed for map renderer"};
        }
        std::this_thread::sleep_for(std::chrono::milliseconds {50});
    }
    if (!finished) {
        kill(pid, SIGKILL);
        waitpid(pid, nullptr, 0);
        throw std::runtime_error {"Map renderer timed out"};
    }
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        throw std::runtime_error {"Map renderer failed; see " + log_path.string()};
    }
    if (!std::filesystem::is_regular_file(image_path)
            || std::filesystem::file_size(image_path) == 0) {
        throw std::runtime_error {"Missing output: " + image_path.string()};
    }
    return image_path;
}
