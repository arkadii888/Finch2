#include "llama_service.hpp"

#include <unistd.h>
#include <sys/wait.h>

#include <chrono>
#include <thread>
#include <vector>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

void LlamaService::Run() {
    client_.set_connection_timeout(10, 0);
    client_.set_read_timeout(300, 0);

    if ((pid_ = fork()) < 0) {
        spdlog::error("LlamaService::Run: Fork failed.");
        return;
    }

    if (pid_ == 0) {
        std::vector<std::string> args {
            "llama-server",
            "-m",         config.model_path,
            "--port",     std::to_string(config.inference_server_port),
            "-c",         std::to_string(backend_config_.context_size),
            "-b",         std::to_string(backend_config_.batch_size),
            "-ub",        std::to_string(backend_config_.ubatch_size),
            "--parallel", std::to_string(backend_config_.parallel),
            "--temp",     std::to_string(backend_config_.temperature),
        };

        if (backend_config_.max_tokens >= 0) {
            args.push_back("-n");
            args.push_back(std::to_string(backend_config_.max_tokens));
        }
        if (backend_config_.gpu_layers > 0) {
            args.push_back("-ngl");
            args.push_back(std::to_string(backend_config_.gpu_layers));
        }
        if (backend_config_.threads >= 0) {
            args.push_back("-t");
            args.push_back(std::to_string(backend_config_.threads));
        }
        if (backend_config_.threads_batch >= 0) {
            args.push_back("-tb");
            args.push_back(std::to_string(backend_config_.threads_batch));
        }
        if (backend_config_.cont_batching) {
            args.push_back("--cont-batching");
        }
        if (!backend_config_.flash_attn.empty()) {
            args.push_back("--flash-attn");
            args.push_back(backend_config_.flash_attn);
        }
        if (backend_config_.mlock) {
            args.push_back("--mlock");
        }
        if (backend_config_.no_mmap) {
            args.push_back("--no-mmap");
        }
        if (backend_config_.log_disable) {
            args.push_back("--log-disable");
        }

        std::vector<char*> argv {};
        for (auto& arg : args) {
            argv.push_back(arg.data());
        }
        argv.push_back(nullptr);

        execv(LLAMA_SERVER_PATH, argv.data());
        perror("Exec failed");
        _exit(1);
    }

    auto deadline {std::chrono::steady_clock::now() + std::chrono::seconds {120}};
    while (std::chrono::steady_clock::now() < deadline) {
        if (waitpid(pid_, nullptr, WNOHANG) > 0) {
            spdlog::error("LlamaService::Run: Llama server crashed.");
            return;
        }

        auto result {client_.Get("/health")};
        if (result && result->status == 200) {
            spdlog::info("LlamaService::Run: Started on port {}", config.inference_server_port);
            return;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds {500});
    }
}

void LlamaService::Stop() {
    if (pid_ > 0) {
        kill(pid_, SIGTERM);
        waitpid(pid_, nullptr, 0);
        spdlog::info("LlamaService::Stop: Stopped.");
    }
}

std::string LlamaService::Complete(const std::string& prompt) {
    nlohmann::json request {
        {"model", config.model_path},
        {"messages", {{{"role", "user"}, {"content", prompt}}}},
        {"stream", false}
    };

    auto result {client_.Post("/v1/chat/completions", request.dump(), "application/json")};
    if (!result || result->status != 200) {
        spdlog::error("LlamaService::Complete: Connection error.");
        return "";
    }

    try {
        auto root = nlohmann::json::parse(result->body);

        if (!root.is_object()) {
            throw std::runtime_error {"expected JSON object"};
        }

        if (root.contains("error")) {
            if (root["error"].is_object()
                    && root["error"].contains("message")
                    && root["error"]["message"].is_string()) {
                throw std::runtime_error {root["error"]["message"].get<std::string>()};
            }
            throw std::runtime_error {root["error"].dump()};
        }

        if (!root["choices"].is_array() || root["choices"].empty()) {
            throw std::runtime_error {"missing choices"};
        }

        if (!root["choices"][0]["message"]["content"].is_string()) {
            throw std::runtime_error {"expected string content"};
        }

        return root["choices"][0]["message"]["content"].get<std::string>();

    } catch (const std::exception& e) {
        spdlog::error("LlamaService::Complete: Error: {}", e.what());
        return "";
    }
}
