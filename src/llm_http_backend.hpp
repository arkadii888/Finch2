#pragma once

#include <stdexcept>
#include <string>

#include <httplib.h>
#include <nlohmann/json.hpp>

class LlmHttpBackend {
 public:
    explicit LlmHttpBackend(int port, std::string model_id)
        : client_ {"127.0.0.1", port},
          model_id_ {std::move(model_id)},
          port_ {port} {
        client_.set_connection_timeout(10, 0);
        client_.set_read_timeout(300, 0);
        client_.set_write_timeout(30, 0);
    }

    std::string Complete(const std::string& prompt) {
        const nlohmann::json request {
            {"model", model_id_},
            {"messages", {{{"role", "user"}, {"content", prompt}}}},
            {"stream", false},
        };

        auto response {client_.Post(
            "/v1/chat/completions",
            httplib::Headers {{"Content-Type", "application/json"}},
            request.dump(),
            "application/json")};

        if (!response) {
            throw std::runtime_error {
                "LlmHttpBackend: could not reach llama-server at 127.0.0.1:"
                + std::to_string(port_)};
        }
        if (response->status != 200) {
            throw std::runtime_error {
                "LlmHttpBackend: HTTP " + std::to_string(response->status) + " — "
                + Truncate(response->body, 200)};
        }

        return ParseAssistantContent(response->body);
    }

 private:
    static std::string Truncate(std::string text, const std::size_t max_len) {
        if (text.size() > max_len) {
            text.resize(max_len);
            text += "...";
        }
        return text;
    }

    static std::string ParseAssistantContent(const std::string& body) {
        nlohmann::json root;
        try {
            root = nlohmann::json::parse(body);
        } catch (const nlohmann::json::exception& e) {
            throw std::runtime_error {
                std::string {"LlmHttpBackend: invalid JSON — "} + e.what() + " — "
                + Truncate(body, 300)};
        }

        if (!root.is_object()) {
            throw std::runtime_error {"LlmHttpBackend: expected JSON object"};
        }

        if (root.contains("error")) {
            const auto& err {root["error"]};
            if (err.is_object() && err.contains("message") && err["message"].is_string()) {
                throw std::runtime_error {
                    "LlmHttpBackend: " + err["message"].get<std::string>()};
            }
            throw std::runtime_error {"LlmHttpBackend: " + err.dump()};
        }

        const auto& choices {root["choices"]};
        if (!choices.is_array() || choices.empty()) {
            throw std::runtime_error {"LlmHttpBackend: missing choices"};
        }

        const auto& content {choices[0]["message"]["content"]};
        if (!content.is_string()) {
            throw std::runtime_error {"LlmHttpBackend: expected string content"};
        }
        return content.get<std::string>();
    }

    httplib::Client client_;
    std::string model_id_;
    int port_;
};
