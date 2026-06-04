#pragma once

#include <string>
#include <sys/types.h>

#include <httplib.h>

#include "llm_service.hpp"
#include "../config.hpp"

#ifndef LLAMA_SERVER_PATH
#error "LLAMA_SERVER_PATH is not defined!"
#endif

class LlamaService : LlmService {
 public:
    void Run() override;
    void Stop() override;

    std::string Complete(const std::string& prompt) override;

 private:
    pid_t pid_ {-1};
    httplib::Client client_ {"127.0.0.1", config.llama_server_port};
};
