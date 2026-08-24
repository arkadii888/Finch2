#pragma once

#include <filesystem>

// Paths and knobs shared by the LLM server and offline map renderer.
struct RuntimeConfig {
    std::filesystem::path dem_path;
    std::filesystem::path inference_log_dir;
    int llama_context_size {8'192};
    int llama_image_tokens {1'550};
    bool llama_jinja {true};
    int llama_max_tokens {2'048};
    double llama_temperature {0.0};
    double map_half_window_m {1'000.0};
    std::filesystem::path map_path;
    int map_zoom {15};
    std::filesystem::path mmproj_path;
    std::filesystem::path model_path;
    std::filesystem::path python_path;
    std::filesystem::path renderer_path;
    int renderer_timeout_s {60};

    static RuntimeConfig Make();

    void Validate() const;
};
