#pragma once

#include <sys/types.h>

#include <string>

#include <httplib.h>

#include "llm_service.hpp"
#include "../config.hpp"

#ifndef LLAMA_SERVER_PATH
#error "LLAMA_SERVER_PATH is not defined!"
#endif

struct LlamaBackendConfig {
    int batch_size = 2'048;
    bool cont_batching = true;
    std::string flash_attn = "on";  // on | off | auto
    bool log_disable = true;
    bool mlock = false;
    bool no_mmap = false;
    int parallel = 1;
    int threads_batch = -1;
    int ubatch_size = 512;
};

inline LlamaBackendConfig llama_backend_config;

class LlamaService : public LlmService {
 public:
    void Run() override;
    void Stop() override;

    std::string Complete(const std::string& prompt) override;

 private:
    pid_t pid_ {-1};
    httplib::Client client_ {"127.0.0.1", config.inference_server_port};
};
