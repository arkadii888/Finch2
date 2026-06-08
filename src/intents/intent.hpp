#pragma once

#include <functional>
#include <string>
#include <vector>

#include <nlohmann/json_fwd.hpp>

#include "drone/drone.hpp"

struct Intent {
    std::string arg_description;
    std::function<std::vector<MissionItem>(const nlohmann::json&)> expand;
};
