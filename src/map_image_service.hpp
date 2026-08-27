#pragma once

#include <filesystem>

#include "runtime_config.hpp"

// Renders an offline map centered on the drone into request_dir/map.png.
std::filesystem::path RenderMapImage(
    const RuntimeConfig& config,
    const double latitude_deg,
    const double longitude_deg,
    const std::filesystem::path& request_dir
);
