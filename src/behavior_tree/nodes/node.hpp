#pragma once

#include <memory>
#include <string>
#include <vector>

enum class NodeStatus {
    Running,
    Success,
    Failure,
};

class Node {
 public:
    virtual ~Node() = default;

    virtual NodeStatus GetStatus() const = 0;
    virtual bool Validate() const = 0;
    virtual std::string GetPrompt() const = 0;

    void AddChild(std::unique_ptr<Node> child);

    const std::vector<std::unique_ptr<Node>>& GetChildrens() const;

 protected:
    std::vector<std::unique_ptr<Node>> childrens_;
};
