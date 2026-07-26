#pragma once

#include <sys/types.h>

#include <string>

#include <httplib.h>

#include "llm_service.hpp"
#include "runtime_config.hpp"

import globals;

#ifndef LLAMA_SERVER_PATH
#error "LLAMA_SERVER_PATH is not defined!"
#endif

class LlamaService : public LlmService {
 public:
    explicit LlamaService(RuntimeConfig config);

    void Run() override;
    void Stop() override;

    std::string Complete(const CompletionRequest& request) override;

 private:
    struct BackendConfig {
        #if defined(LLAMA_BACKEND_METAL) || defined(LLAMA_BACKEND_CUDA)
            int gpu_layers {999};
        #else
            int gpu_layers {0};
        #endif

        int batch_size {2'048};
        bool cont_batching {true};
        std::string flash_attn {"on"};  // on | off | auto
        bool log_disable {true};
        bool mlock {false};
        bool no_mmap {false};
        int parallel {1};
        int threads {-1};
        int threads_batch {-1};
        int ubatch_size {512};
    };

    BackendConfig backend_config_ {};
    httplib::Client client_ {"127.0.0.1", globals::llm_server_port};
    RuntimeConfig config_;
    pid_t pid_ {-1};
};
