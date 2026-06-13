#include "node_descriptors.hpp"

std::vector<NodeDescriptorRegistry::Provider>& NodeDescriptorRegistry::providers() {
    static std::vector<Provider> p;
    return p;
}

void NodeDescriptorRegistry::Register(Provider provider) {
    providers().push_back(provider);
}

std::vector<NodeDescriptor> NodeDescriptorRegistry::GetAll() {
    std::vector<NodeDescriptor> result;
    result.reserve(providers().size());
    for (const Provider provider : providers()) {
        result.push_back(provider());
    }
    return result;
}

std::vector<NodeDescriptor> GetNodeDescriptors() {
    return NodeDescriptorRegistry::GetAll();
}
