#include "runtime_config.hpp"

#include <stdexcept>
#include <string>

#ifndef PROJECT_ROOT_DIR
#error "PROJECT_ROOT_DIR is not defined!"
#endif

namespace {

void RequireDirectory(const std::filesystem::path& path, const char* name) {
    if (!std::filesystem::is_directory(path)) {
        throw std::runtime_error {
            std::string {name} + " directory not found: " + path.string()
        };
    }
}

void RequireFile(const std::filesystem::path& path, const char* name) {
    if (!std::filesystem::is_regular_file(path)) {
        throw std::runtime_error {
            std::string {name} + " file not found: " + path.string()
        };
    }
}

}  // namespace

RuntimeConfig RuntimeConfig::Make() {
    const std::filesystem::path root {PROJECT_ROOT_DIR};
    RuntimeConfig config;
    config.dem_path = root / "data/maps/output_hh.tif";
    config.inference_log_dir = root / "inference_runs";
    config.mmproj_path = root / "models/mmproj-Qwen3VL-8B-Instruct-F16.gguf";
    config.model_path = root / "models/Qwen3VL-8B-Instruct-Q4_K_M.gguf";
    config.python_path = root / "tools/map_renderer/.venv/bin/python";
    config.renderer_path = root / "tools/map_renderer/cli.py";
    config.tile_cache_path = root / "data/maps/tiles/osm_de";
    return config;
}

void RuntimeConfig::Validate() const {
    RequireFile(dem_path, "DEM");
    RequireFile(mmproj_path, "Multimodal projector");
    RequireFile(model_path, "Model");
    RequireFile(python_path, "Python interpreter");
    RequireFile(renderer_path, "Map renderer");
    RequireDirectory(tile_cache_path, "Tile cache");
}
