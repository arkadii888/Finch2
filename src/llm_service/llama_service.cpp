#include "llama_service.hpp"

#include <unistd.h>

#include <vector>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

void LlamaService::Run() {
    client_.set_connection_timeout(10, 0);
    client_.set_read_timeout(300, 0);

    if ((pid_ = fork()) < 0) {
        spdlog::error("LlmService::Run: Fork failed.");
        return;
    }

    if (pid_ == 0) {
        const InferenceConfig& inference {config.inference};
        const LlamaBackendConfig& llama {llama_backend_config};

        std::vector<std::string> args {
            "llama-server",
            "-m",         config.model_path,
            "--port",     std::to_string(config.inference_server_port),
            "-c",         std::to_string(inference.context_size),
            "-b",         std::to_string(llama.batch_size),
            "-ub",        std::to_string(llama.ubatch_size),
            "--parallel", std::to_string(llama.parallel),
            "--temp",     std::to_string(inference.temperature),
        };

        auto flag = [&](bool cond, auto... a) {
            if (cond) (args.push_back(std::string{a}), ...);
        };

        flag(inference.max_tokens >= 0,  "-n",   std::to_string(inference.max_tokens));
        flag(inference.gpu_layers > 0,   "-ngl", std::to_string(inference.gpu_layers));
        flag(llama.threads >= 0,        "-t",   std::to_string(llama.threads));
        flag(llama.threads_batch >= 0,  "-tb",  std::to_string(llama.threads_batch));
        flag(llama.cont_batching,       "--cont-batching");
        flag(!llama.flash_attn.empty(), "--flash-attn", llama.flash_attn);
        flag(llama.mlock,               "--mlock");
        flag(llama.no_mmap,             "--no-mmap");
        flag(llama.log_disable,         "--log-disable");

        std::vector<char*> argv;
        for (auto& arg : args) argv.push_back(arg.data());
        argv.push_back(nullptr);

        execv(LLAMA_SERVER_PATH, argv.data());
        perror("Exec failed");
        _exit(1);
    }

    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(120);
    while (std::chrono::steady_clock::now() < deadline) {
        if (waitpid(pid_, nullptr, WNOHANG) > 0) {
            spdlog::error("LlmService::Run: Llama server crashed.");
            return;
        }

        auto result {client_.Get("/health")};
        if(result && result->status == 200) {
            spdlog::info("LlmService::Run: Started on port {}", config.inference_server_port);
            return;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void LlamaService::Stop() {
    if (pid_ > 0) {
        kill(pid_, SIGTERM);
        waitpid(pid_, nullptr, 0);
        spdlog::info("LlmService::~Stop: Stopped.");
    }
}

std::string LlamaService::Complete(const std::string& prompt) {
    nlohmann::json request = {
        {"model", config.model_path},
        {"messages", {{{"role", "user"}, {"content", prompt}}}},
        {"stream", false}
    };

    auto result {client_.Post("/v1/chat/completions", request.dump(), "application/json")};
    if (!result || result->status != 200) {
        spdlog::error("LlmService::Complete: Connection error.");
        return "";
    }

    try {
        auto root = nlohmann::json::parse(result->body);

        if (!root.is_object()) {
            throw std::runtime_error {"expected JSON object"};
        }

        if (root.contains("error")) {
            const auto& err {root["error"]};
            if (err.is_object() && err.contains("message") && err["message"].is_string()) {
                throw std::runtime_error {err["message"].get<std::string>()};
            }
            throw std::runtime_error {err.dump()};
        }

        const auto& choices {root["choices"]};
        if (!choices.is_array() || choices.empty()) {
            throw std::runtime_error {"missing choices"};
        }

        const auto& content {choices[0]["message"]["content"]};
        if (!content.is_string()) {
            throw std::runtime_error {"expected string content"};
        }

        return content.get<std::string>();

    } catch (const std::exception& e) {
        spdlog::error("LlmService::Complete: Error: {}", e.what());
        return "";
    }
}
