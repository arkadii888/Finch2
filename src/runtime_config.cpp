#include "runtime_config.hpp"

#include <stdexcept>
#include <string>

#ifndef PROJECT_ROOT_DIR
#error "PROJECT_ROOT_DIR is not defined!"
#endif

namespace {

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
    config.dem_path = root / "data/maps/switzerland.tif";
    config.inference_log_dir = root / "inference_runs";
    config.map_path = root / "data/maps/switzerland.gpkg";
    config.mmproj_path = root / "data/models/mmproj-Qwen3VL-8B-Instruct-F16.gguf";
    config.model_path = root / "data/models/Qwen3VL-8B-Instruct-Q4_K_M.gguf";
    config.python_path = root / "tools/map_renderer/.venv/bin/python";
    config.renderer_path = root / "tools/map_renderer/cli.py";
    return config;
}

void RuntimeConfig::Validate() const {
    RequireFile(dem_path, "DEM");
    RequireFile(map_path, "Map GeoPackage");
    RequireFile(mmproj_path, "Multimodal projector");
    RequireFile(model_path, "Model");
    RequireFile(python_path, "Python interpreter");
    RequireFile(renderer_path, "Map renderer");
}
