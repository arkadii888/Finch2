#include "llama_service.hpp"

#include <sys/wait.h>
#include <unistd.h>

#include <chrono>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace {

std::string Base64Encode(const std::vector<unsigned char>& data) {
    constexpr char ALPHABET[] {
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
    };
    std::string encoded;
    encoded.reserve(((data.size() + 2) / 3) * 4);
    for (std::size_t index {0}; index < data.size(); index += 3) {
        const unsigned int first {data[index]};
        const unsigned int second {index + 1 < data.size() ? data[index + 1] : 0U};
        const unsigned int third {index + 2 < data.size() ? data[index + 2] : 0U};
        const unsigned int value {(first << 16U) | (second << 8U) | third};
        encoded.push_back(ALPHABET[(value >> 18U) & 0x3fU]);
        encoded.push_back(ALPHABET[(value >> 12U) & 0x3fU]);
        encoded.push_back(index + 1 < data.size() ? ALPHABET[(value >> 6U) & 0x3fU] : '=');
        encoded.push_back(index + 2 < data.size() ? ALPHABET[value & 0x3fU] : '=');
    }
    return encoded;
}

std::string EncodeImage(const std::filesystem::path& path) {
    std::ifstream input {path, std::ios::binary};
    if (!input) {
        throw std::runtime_error {"Could not read map image: " + path.string()};
    }
    const std::vector<unsigned char> data {
        std::istreambuf_iterator<char> {input},
        std::istreambuf_iterator<char> {}
    };
    if (data.empty()) {
        throw std::runtime_error {"Map image is empty: " + path.string()};
    }
    return Base64Encode(data);
}

}  // namespace

LlamaService::LlamaService(RuntimeConfig config) : config_ {std::move(config)} {}

void LlamaService::Run() {
    client_.set_connection_timeout(10, 0);
    client_.set_read_timeout(300, 0);

    if ((pid_ = fork()) < 0) {
        throw std::runtime_error {"Could not fork llama-server"};
    }

    if (pid_ == 0) {
        std::vector<std::string> args {
            "llama-server",
            "-m",         config_.model_path.string(),
            "--mmproj",   config_.mmproj_path.string(),
            "--port",     std::to_string(globals::llm_server_port),
            "-c",         std::to_string(config_.llama_context_size),
            "-n",         std::to_string(config_.llama_max_tokens),
            "-b",         std::to_string(backend_config_.batch_size),
            "-ub",        std::to_string(backend_config_.ubatch_size),
            "--parallel", std::to_string(backend_config_.parallel),
            "--temp",     "0",
            "--jinja",
            "--image-min-tokens", std::to_string(config_.llama_image_tokens),
            "--image-max-tokens", std::to_string(config_.llama_image_tokens),
        };

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
            pid_ = -1;
            throw std::runtime_error {"llama-server crashed during startup"};
        }

        auto result {client_.Get("/health")};
        if (result && result->status == 200) {
            spdlog::info(
                "LlamaService::Run: Multimodal server started on port {}.",
                globals::llm_server_port
            );
            return;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds {500});
    }
    Stop();
    throw std::runtime_error {"llama-server startup timed out"};
}

void LlamaService::Stop() {
    if (pid_ > 0) {
        kill(pid_, SIGTERM);
        waitpid(pid_, nullptr, 0);
        pid_ = -1;
        spdlog::info("LlamaService::Stop: Stopped.");
    }
}

std::string LlamaService::Complete(const CompletionRequest& request) {
    try {
        nlohmann::json user_content = nlohmann::json::array();
        user_content.push_back({
            {"type", "text"},
            {"text", request.user_prompt}
        });
        if (request.image_path) {
            user_content.push_back({
                {"type", "image_url"},
                {"image_url", {
                    {"url", "data:image/png;base64," + EncodeImage(*request.image_path)}
                }}
            });
        }
        const nlohmann::json body {
            {"model", config_.model_path.string()},
            {"messages", {
                {{"role", "system"}, {"content", request.system_prompt}},
                {{"role", "user"},   {"content", user_content}},
            }},
            {"response_format", {{"type", "json_object"}}},
            {"stream", false},
            {"temperature", 0.0}
        };

        auto result {
            client_.Post("/v1/chat/completions", body.dump(), "application/json")
        };
        if (!result) {
            throw std::runtime_error {"connection error"};
        }
        if (result->status != 200) {
            throw std::runtime_error {
                "HTTP " + std::to_string(result->status) + ": " + result->body
            };
        }

        nlohmann::json root = nlohmann::json::parse(result->body);

        if (!root.is_object()) {
            throw std::runtime_error {
                std::string {"expected JSON object, received "} + root.type_name()
            };
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

        auto output {root["choices"][0]["message"]["content"].get<std::string>()};
        spdlog::info("LlamaService::Complete: {}", output);
        return output;
    } catch (const std::exception& error) {
        spdlog::error("LlamaService::Complete: Error: {}", error.what());
        throw;
    }
}
