#pragma once

#include <memory>

#include "nodes/node.hpp"

class NodeCatalog {
 public:
    NodeCatalog();

    const std::vector<std::unique_ptr<Node>>& GetNodes() const;

 private:
    std::vector<std::unique_ptr<Node>> nodes_;
};
