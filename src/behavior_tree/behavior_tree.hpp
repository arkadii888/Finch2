#pragma once

#include <string>

#include <nlohmann/json_fwd.hpp>

class IntentCatalog;

class BehaviorTree {
 public:
    static std::string ValidateNode(const nlohmann::json& node, const IntentCatalog& catalog);
};
