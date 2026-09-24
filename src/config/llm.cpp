#include "llm.hpp"

#include <cstdlib>
#include <spdlog/spdlog.h>


LlmConfig::LlmConfig() {
    Validate();
}

void LlmConfig::Validate() const {
    RequireFile(btree_grammar_path);
    RequireFile(dem_path);
    RequireFile(map_path);
    RequireFile(mmproj_path);
    RequireFile(model_path);
    RequireFile(python_path);
    RequireFile(renderer_path);
}

void LlmConfig::RequireFile(const std::filesystem::path& path) const {
    if (!std::filesystem::is_regular_file(path)) {
        spdlog::critical("LlmConfig::RequireFile: File not found {}",  path.string());
        std::exit(EXIT_FAILURE);
    }
}
