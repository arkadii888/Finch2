#pragma once

#include <string>

#ifndef PROJECT_ROOT_DIR
#error "PROJECT_ROOT_DIR is not defined!"
#endif

struct InferenceConfig {
    int context_size = 4096;
    int max_tokens = -1;  // -1 = unlimited
    float temperature = 0.8f;

    #if defined(FINCH_BACKEND_METAL) || defined(FINCH_BACKEND_CUDA)
        int gpu_layers = 999;
    #else
        int gpu_layers = 0;
    #endif

    int threads = -1;  // -1 = let backend decide
};

struct Config {
    const int http_server_port = 8888;
    const int inference_server_port = 8889;
    const std::string model_path =
        std::string {PROJECT_ROOT_DIR} + "/models/gemma-4-E2B-it-Q4_K_M.gguf";
    InferenceConfig inference;
};

inline Config config;
