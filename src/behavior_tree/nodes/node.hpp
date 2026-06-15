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
    Node(std::string name);
    virtual ~Node() = default;

    virtual bool Validate() const = 0;
    virtual NodeStatus GetStatus() const = 0;
    virtual std::string GetPrompt() const = 0;

    void AddChild(std::unique_ptr<Node> child);

    const std::string& GetName() const;
    const std::vector<std::unique_ptr<Node>>& GetChildren() const;

 protected:
    std::string name_;
    std::vector<std::unique_ptr<Node>> children_;
};
