#pragma once

#include <filesystem>

#include "config/llm.hpp"

// Renders an offline map centered on the drone into request_dir/map.png.
std::filesystem::path RenderMapImage(
    const LlmConfig& config,
    const double latitude_deg,
    const double longitude_deg,
    const std::filesystem::path& request_dir
);
