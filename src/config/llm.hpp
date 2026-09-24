#pragma once

#include <filesystem>

struct LlmConfig {
    bool llama_jinja {true};
    double llama_temperature {0.0};
    double map_half_window_m {1'000.0};
    int llama_context_size {16'384};
    int llama_image_tokens {2'304};
    int llama_max_tokens {6'000};
    int map_zoom {15};
    int max_go_to {26};
    int renderer_timeout_s {60};
    std::filesystem::path btree_grammar_path;
    std::filesystem::path dem_path;
    std::filesystem::path inference_log_dir;
    std::filesystem::path map_path;
    std::filesystem::path mmproj_path;
    std::filesystem::path model_path;
    std::filesystem::path python_path;
    std::filesystem::path renderer_path;

    static LlmConfig Make();

    void Validate() const;
};
