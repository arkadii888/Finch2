#include "llm_service.hpp"

#include <chrono>
#include <cstdio>
#include <iostream>
#include <signal.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

#include <nlohmann/json.hpp>

LlmService::LlmService(const std::string& model_path, int port) : model_path_ {model_path}, client_ {"127.0.0.1", port} {
    client_.set_connection_timeout(10, 0);
    client_.set_read_timeout(300, 0);

    if ((pid_ = fork()) < 0) {
        std::cout << "LlmService::LlmService: Fork failed." << std::endl;
        return;
    }

    if (pid_ == 0) {
        execl(LLAMA_SERVER_PATH, "llama-server", "-m", model_path.c_str(),
                  "--port", std::to_string(port).c_str(), "-c", "4096", nullptr);
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
        {"model", model_path_},
        {"messages", {{{"role", "user"}, {"content", prompt}}}},
        {"stream", false}
    };

    auto result {client_.Post("/v1/chat/completions", request.dump(), "application/json")};
    if (!result || result->status != 200) {
        std::cout << "LlmService::Complete: Http connection error." << std::endl;
        return "";
    }

    auto root {nlohmann::json::parse(result->body, nullptr, false)};
    if (root.is_discarded() || root.contains("error")) {
        std::cout << "LlmService::Complete: Json error." << std::endl;
        return "";
    }

    return root["choices"][0]["message"]["content"].get<std::string>();
}
