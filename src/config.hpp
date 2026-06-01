#pragma once

#include <string>

struct Config {
    const int http_server_port = 8888;
    const int llama_server_port = 8889;
    const std::string model_path = "/home/arkadii/Documents/Projects/CxxProjects/Finch2/models/gemma-4-E2B-it-Q4_K_M.gguf";
};

inline Config config;
