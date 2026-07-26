#pragma once

#include <filesystem>

#include "runtime_config.hpp"
#include "vehicle/vehicle.hpp"

// Renders an offline map centered on the drone into request_dir/map.png.
std::filesystem::path RenderMapImage(
    const RuntimeConfig& config,
    const Telemetry& telemetry,
    const std::filesystem::path& request_dir
);
