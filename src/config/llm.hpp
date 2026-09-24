#pragma once

#include <filesystem>

struct LlmConfig {
    std::filesystem::path btree_grammar_path;
    std::filesystem::path dem_path;
    std::filesystem::path inference_log_dir;
    int llama_context_size {16'384};
    int llama_image_tokens {2'304};
    bool llama_jinja {true};
    int llama_max_tokens {6'000};
    double llama_temperature {0.0};
    double map_half_window_m {1'000.0};
    std::filesystem::path map_path;
    int map_zoom {15};
    int max_go_to {26};
    std::filesystem::path mmproj_path;
    std::filesystem::path model_path;
    std::filesystem::path python_path;
    std::filesystem::path renderer_path;
    int renderer_timeout_s {60};

    static LlmConfig Make();

    void Validate() const;
};
