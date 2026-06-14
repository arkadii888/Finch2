#include "action_node.hpp"

Action::Action(std::string name) : Node {std::move(name)} {}

bool Action::Validate() const {
    return true;
}
