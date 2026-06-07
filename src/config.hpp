#pragma once

#include <string>

#ifndef PROJECT_ROOT_DIR
#error "PROJECT_ROOT_DIR is not defined!"
#endif

struct Config {
    const int http_server_port {8888};
    const int inference_server_port {8889};
    const std::string model_path {
        std::string {PROJECT_ROOT_DIR} + "/models/gemma-4-E2B-it-Q4_K_M.gguf"
    };
};

inline Config config;
