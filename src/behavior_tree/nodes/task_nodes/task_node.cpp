#include "task_node.hpp"

#include <stdexcept>

Task::Task(std::string name) : Action {std::move(name)} {}

NodeStatus Task::GetStatus() const {
    return Tick();
}

ComputeFibonacci::ComputeFibonacci(int target_n)
    : Task {"compute_fibonacci"}, target_n_ {target_n} {}

std::string ComputeFibonacci::GetPrompt() {
    return R"(action/compute_fibonacci: {"type": "action", "compute_fibonacci": {"n": <int>}})";
}

bool ComputeFibonacci::Validate() const {
    return target_n_ > 0;
}

NodeStatus ComputeFibonacci::Tick() const {
    const auto now {std::chrono::steady_clock::now()};

    if (!started_) {
        start_time_ = now;
        started_ = true;
        return NodeStatus::Running;
    }

    const auto elapsed_ms {std::chrono::duration_cast<std::chrono::milliseconds>(
        now - start_time_)
                               .count()};
    if (elapsed_ms >= static_cast<long long>(target_n_) * kPauseMs) {
        return NodeStatus::Success;
    }
    return NodeStatus::Running;
}

int ComputeFibonacci::GetTargetN() const { return target_n_; }

std::unique_ptr<Node> ComputeFibonacci::FromJson(const nlohmann::json& args) {
    if (!args.contains("n") || !args["n"].is_number_integer()) {
        throw std::runtime_error {
            "compute_fibonacci action requires an integer \"n\" field"};
    }
    return std::make_unique<ComputeFibonacci>(args["n"].get<int>());
}
