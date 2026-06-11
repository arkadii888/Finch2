#include "intents/intent_catalog.hpp"

#include <nlohmann/json.hpp>

std::vector<MissionItem> ExpandMoveTo(const nlohmann::json& node) {
    MissionItem item {};
    item.latitude_deg = node["move_to"]["lat"];
    item.longitude_deg = node["move_to"]["lon"];
    return {item};
}

Intent MakeMoveToIntent() {
    return Intent {
        R"({"lat": <degrees float>, "lon": <degrees float>})",
        ExpandMoveTo,
    };
}

void IntentCatalog::Register(std::string name, Intent intent) {
    intents_.emplace(std::move(name), std::move(intent));
}

const Intent* IntentCatalog::Find(const std::string& name) const {
    const auto it {intents_.find(name)};
    return it == intents_.end() ? nullptr : &it->second;
}

const std::unordered_map<std::string, Intent>& IntentCatalog::GetIntents() const {
    return intents_;
}

IntentCatalog IntentCatalog::MakeDefault() {
    IntentCatalog catalog;
    catalog.Register("move_to", MakeMoveToIntent());
    return catalog;
}
