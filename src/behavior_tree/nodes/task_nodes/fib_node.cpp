#include "fib_node.hpp"

FibNode::FibNode(int n) : n_(n) {}

void FibNode::Execute(std::any context) {
    TaskNode::Execute(context);

    if (status_ != NodeStatus::Running) {
        return;
    }

    if (index_ >= n_) {
        status_ = NodeStatus::Success;
        return;
    }

    const auto now {std::chrono::steady_clock::now()};

    if (!started_) {
        started_ = true;
        last_step_time_ = now;
        return;
    }

    if (now - last_step_time_ < std::chrono::seconds {1}) {
        return;
    }

    const long long next {a_ + b_};
    a_ = b_;
    b_ = next;
    ++index_;
    last_step_time_ = now;

    if (index_ >= n_) {
        status_ = NodeStatus::Success;
    }
}

NodeStatus FibNode::GetStatus() {
    return status_;
}

bool FibNode::Validate() const {
    return n_ > 0;
}

std::string FibNode::GetPrompt() const {
    return R"({"type": "action", "fib": {
        "n": <int>}})";
}
