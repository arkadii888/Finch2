#include "parallel_node.hpp"

ParallelNode::ParallelNode(int success_threshold) : success_threshold_ {success_threshold} {}

NodeStatus ParallelNode::GetStatus() const {
    return status_;
}

bool ParallelNode::Validate() const {
    if (childrens_.empty()) {
        return false;
    }
    if (success_threshold_ < 1
            || success_threshold_ > static_cast<int>(childrens_.size())) {
        return false;
    }
    return true;
}

std::string ParallelNode::GetPrompt() const {
    return R"(parallel: {"type": "parallel", "success_threshold": 1, "children": [...]})";
}

int ParallelNode::GetSuccessThreshold() const {
    return success_threshold_;
}
