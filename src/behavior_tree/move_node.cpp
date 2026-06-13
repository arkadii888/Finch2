#include "move_node.hpp"

#include <map>
#include <stdexcept>

#include "node_descriptors.hpp"

std::map<std::string, MoveFactory>& MoveRegistry::entries() {
    static std::map<std::string, MoveFactory> m;
    return m;
}

void MoveRegistry::Register(std::string key, MoveFactory factory) {
    entries().emplace(std::move(key), std::move(factory));
}

MoveFactory* MoveRegistry::Find(const std::string& key) {
    auto it = entries().find(key);
    return it != entries().end() ? &it->second : nullptr;
}

Move::Move(std::string name) : Action {std::move(name)} {}

void Move::SetMissionRange(int start_idx, int count) {
    start_idx_ = start_idx;
    count_ = count;
}

void Move::UpdateProgress(int current_wp) {
    cached_status_ = (current_wp >= start_idx_ + count_)
        ? NodeStatus::Success
        : NodeStatus::Running;
}

NodeStatus Move::GetStatus() const {
    return cached_status_;
}

MoveTo::MoveTo(double lat, double lon, float alt_m)
    : Move {"move_to"}, lat_ {lat}, lon_ {lon}, alt_m_ {alt_m} {}

NodeDescriptor MoveTo::Descriptor() {
    return {
        "action/move_to",
        R"({"type": "action", "move_to": {"lat": <degrees_float>, "lon": <degrees_float>, "alt_m": <meters_float>}})",
    };
}

std::vector<MissionItem> MoveTo::GetMissionItem() const {
    MissionItem item {};
    item.latitude_deg = lat_;
    item.longitude_deg = lon_;
    item.relative_altitude_m = alt_m_;
    item.is_fly_through = false;
    return {item};
}

bool MoveTo::Validate() const {
    if (lat_ < -90.0 || lat_ > 90.0) {
        return false;
    }
    if (lon_ < -180.0 || lon_ > 180.0) {
        return false;
    }
    return true;
}

std::unique_ptr<Node> MoveTo::FromJson(
    const nlohmann::json& args, std::vector<MissionItem>& mission_items)
{
    if (!args.contains("lat") || !args.contains("lon")) {
        throw std::runtime_error {"move_to action requires \"lat\" and \"lon\" fields"};
    }

    auto move = std::make_unique<MoveTo>(
        args["lat"].get<double>(),
        args["lon"].get<double>(),
        args.value("alt_m", 0.0f));

    const int start {static_cast<int>(mission_items.size())};
    std::vector<MissionItem> items {move->GetMissionItem()};
    const int count {static_cast<int>(items.size())};
    for (auto& item : items) {
        mission_items.push_back(std::move(item));
    }
    move->SetMissionRange(start, count);
    return move;
}

double MoveTo::GetLat() const { return lat_; }
double MoveTo::GetLon() const { return lon_; }
float MoveTo::GetAltM() const { return alt_m_; }

REGISTER_NODE_DESCRIPTOR(MoveTo)
REGISTER_MOVE_ACTION("move_to", MoveTo)
