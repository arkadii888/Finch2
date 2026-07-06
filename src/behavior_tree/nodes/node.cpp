#include "node.hpp"

void Node::AddChild(std::unique_ptr<Node> child) {
    if (child == nullptr) {
        return;
    }
    childrens_.push_back(std::move(child));
}

const std::vector<std::unique_ptr<Node>>& Node::GetChildrens() const {
    return childrens_;
}
