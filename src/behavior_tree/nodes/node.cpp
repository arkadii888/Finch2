#include "node.hpp"

bool Node::Validate() const {
    for (const auto& child : children_) {
        if (!child->Validate()) {
            return false;
        }
    }
    return true;
}

void Node::AddChild(std::unique_ptr<Node> child) {
    children_.push_back(std::move(child));
}

const std::vector<std::unique_ptr<Node>>& Node::GetChildren() const {
    return children_;
}
