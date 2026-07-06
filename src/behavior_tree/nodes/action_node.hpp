#pragma once

#include <any>

#include "node.hpp"

class ActionNode : public Node {
 public:
    virtual void Execute(std::any context) = 0;
};
