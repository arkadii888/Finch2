#include "llm_service.hpp"

#include <chrono>
#include <cstdio>
#include <format>
#include <iostream>
#include <signal.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

#include <nlohmann/json.hpp>

#include "config.hpp"

LlmService::LlmService() : client_ {"127.0.0.1", config.llama_server_port} {
    client_.set_connection_timeout(10, 0);
    client_.set_read_timeout(300, 0);

    if ((pid_ = fork()) < 0) {
        std::cout << "LlmService::LlmService: Fork failed." << std::endl;
        return;
    }

    if (pid_ == 0) {
        execl(LLAMA_SERVER_PATH, "llama-server", "-m", config.model_path.c_str(),
                  "--port", std::to_string(config.llama_server_port).c_str(), "-c", "4096", nullptr);
        perror("Exec failed");
        _exit(1);
    }

    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(120);
    while (std::chrono::steady_clock::now() < deadline) {
        if (waitpid(pid_, nullptr, WNOHANG) > 0) {
            std::cout << "LlmService::LlmService: Llama server crashed." << std::endl;
            return;
        }

        auto result {client_.Get("/health")};
        if(result && result->status == 200) {
            std::cout << "LlmService::LlmService: Llama server started on port "
            << config.llama_server_port << "." << std::endl;
            return;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

LlmService::~LlmService() {
    if (pid_ > 0) {
        kill(pid_, SIGTERM);
        waitpid(pid_, nullptr, 0);
    }
}

std::string LlmService::Complete(const std::string& prompt) {
    nlohmann::json request = {
        {"model", config.model_path},
        {"messages", {{{"role", "user"}, {"content", prompt}}}},
        {"stream", false}
    };

    auto result {client_.Post("/v1/chat/completions", request.dump(), "application/json")};
    if (!result || result->status != 200) {
        std::cout << "LlmService::Complete: Http connection error." << std::endl;
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
        std::cout << "LlmService::Complete: Error: " << e.what() << "\n"
                  << "Raw response body: " << result->body << std::endl;
        return "";
    }
}
