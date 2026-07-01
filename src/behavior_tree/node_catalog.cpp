#include "node_catalog.hpp"

#include "behavior_tree/nodes/move_nodes/go_to_node.hpp"
#include "behavior_tree/nodes/sequence_node.hpp"
#include "behavior_tree/nodes/fallback_node.hpp"
#include "behavior_tree/nodes/parallel_node.hpp"

NodeCatalog::NodeCatalog() {
    nodes_.push_back(std::make_unique<SequenceNode>());
    nodes_.push_back(std::make_unique<FallbackNode>());
    nodes_.push_back(std::make_unique<ParallelNode>(1));

    nodes_.push_back(std::make_unique<GoToNode>(1.0, 1.0, 1.f, 1.f));
}

const std::vector<std::unique_ptr<Node>>& NodeCatalog::GetNodes() const {
    return nodes_;
}
