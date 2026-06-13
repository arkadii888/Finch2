#pragma once

#include <vector>

#include "node.hpp"

class NodeDescriptorRegistry {
 public:
    using Provider = NodeDescriptor (*)();

    static void Register(Provider provider);
    static std::vector<NodeDescriptor> GetAll();

 private:
    static std::vector<Provider>& providers();
};

struct NodeDescriptorRegistrar {
    explicit NodeDescriptorRegistrar(NodeDescriptorRegistry::Provider provider) {
        NodeDescriptorRegistry::Register(provider);
    }
};

#define REGISTER_NODE_DESCRIPTOR(Class) \
    static const NodeDescriptorRegistrar Class##_registrar {&Class::Descriptor};

std::vector<NodeDescriptor> GetNodeDescriptors();
