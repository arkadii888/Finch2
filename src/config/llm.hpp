#pragma once

#include <filesystem>

#ifndef PROJECT_ROOT_DIR
#error "PROJECT_ROOT_DIR is not defined!"
#endif

class LlmConfig {
 public:
    LlmConfig();

    bool llama_jinja {true};
    double llama_temperature {0.0};
    double map_half_window_m {1'000.0};
    int llama_context_size {16'384};
    int llama_image_tokens {2'304};
    int llama_max_tokens {6'000};
    int map_zoom {15};
    int max_go_to {26};
    int renderer_timeout_s {60};

    std::filesystem::path btree_grammar_path {root / "data/grammars/btree_pixel.gbnf"};
    std::filesystem::path dem_path {root / "data/maps/switzerland.tif"};
    std::filesystem::path inference_log_dir {root / "inference_runs"};
    std::filesystem::path map_path {root / "data/maps/switzerland.gpkg"};
    std::filesystem::path mmproj_path {root / "data/models/mmproj-Qwen3VL-8B-Instruct-F16.gguf"};
    std::filesystem::path model_path {root / "data/models/Qwen3VL-8B-Instruct-Q4_K_M.gguf"};
    std::filesystem::path python_path {root / "tools/map_renderer/.venv/bin/python"};
    std::filesystem::path renderer_path {root / "tools/map_renderer/cli.py"};

 private:
    void Validate() const;
    void RequireFile(const std::filesystem::path& path) const;

    inline static const std::filesystem::path root {PROJECT_ROOT_DIR};
};

// default mmproj_path = mmproj-Qwen3VL-8B-Instruct-F16.gguf
// default model_path = Qwen3VL-8B-Instruct-Q4_K_M.gguf

// trained mmproj_path = qwen3-vl-8b-instruct.BF16-mmproj.gguf
// trained model_path = qwen3-vl-8b-instruct.Q4_K_M.gguf
