#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "action_node.hpp"
#include "drone/drone.hpp"

using MoveFactory = std::function<
    std::unique_ptr<Node>(const nlohmann::json&, std::vector<MissionItem>&)>;

class MoveRegistry {
 public:
    static void Register(std::string key, MoveFactory factory);
    static MoveFactory* Find(const std::string& key);

 private:
    static std::map<std::string, MoveFactory>& entries();
};

struct MoveRegistrar {
    MoveRegistrar(std::string key, MoveFactory factory) {
        MoveRegistry::Register(std::move(key), std::move(factory));
    }
};

#define REGISTER_MOVE_ACTION(key, Class) \
    static const MoveRegistrar Class##_move_registrar {key, &Class::FromJson};

class Move : public Action {
 public:
    explicit Move(std::string name);

    void SetMissionRange(int start_idx, int count);
    void UpdateProgress(int current_wp);

    virtual std::vector<MissionItem> GetMissionItem() const = 0;

    bool Validate() const override = 0;

    NodeStatus GetStatus() const override;

 protected:
    int count_ {0};
    int start_idx_ {0};
    NodeStatus cached_status_ {NodeStatus::Running};
};

class MoveTo : public Move {
 public:
    MoveTo(double lat, double lon, float alt_m);
    static NodeDescriptor Descriptor();
    static std::unique_ptr<Node> FromJson(
        const nlohmann::json& args, std::vector<MissionItem>& mission_items);
    std::vector<MissionItem> GetMissionItem() const override;
    bool Validate() const override;

    double GetLat() const;
    double GetLon() const;
    float GetAltM() const;

 private:
    double lat_;
    double lon_;
    float alt_m_;
};
