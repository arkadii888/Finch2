#pragma once

#include <memory>
#include <string>
#include <vector>

enum class NodeStatus {
    Running,
    Success,
    Failure,
};

struct NodeDescriptor {
    std::string type_name;
    std::string json_example;
};

class Node {
 public:
    explicit Node(std::string name);
    virtual ~Node() = default;

    virtual bool Validate() const;

    virtual NodeStatus GetStatus() const = 0;

    void AddChild(std::unique_ptr<Node> child);

    const std::string& GetName() const;
    const std::vector<std::unique_ptr<Node>>& GetChildren() const;

 protected:
    std::string name_;
    std::vector<std::unique_ptr<Node>> children_;
};
