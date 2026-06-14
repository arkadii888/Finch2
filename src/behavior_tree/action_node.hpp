#pragma once

#include <string>

#include "node.hpp"

class Action : public Node {
 public:
    explicit Action(std::string name);
    bool Validate() const override;
};
