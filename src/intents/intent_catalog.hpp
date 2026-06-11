#pragma once

#include <string>
#include <unordered_map>

#include "intents/intent.hpp"

class IntentCatalog {
 public:
    static IntentCatalog MakeDefault();

    void Register(std::string name, Intent intent);

    const Intent* Find(const std::string& name) const;
    const std::unordered_map<std::string, Intent>& GetIntents() const;

 private:
    std::unordered_map<std::string, Intent> intents_;
};
