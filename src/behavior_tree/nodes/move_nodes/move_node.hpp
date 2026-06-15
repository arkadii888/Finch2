#pragma once

#include <memory>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "behavior_tree/nodes/action_node.hpp"
#include "drone/drone.hpp"

class MoveNode : public ActionNode {
 public:
    explicit MoveNode(std::string name);

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

class MoveTo : public MoveNode {
 public:
    MoveTo(double lat, double lon, float alt_m);
    static std::string GetPrompt();
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
