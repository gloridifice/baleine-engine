#pragma once

#include "VkBootstrap.h"
#include "baleine_type/memory.h"

namespace balkan {
class Instance : EnableSharedFromThis<Instance> {
    friend class RenderState;

    VkInstance instance;
    vkb::Instance vkb_instance;

  public:
    explicit Instance(const char* app_name);
    ~Instance();

    VkInstance get_vulkan_instance() const {
        return instance;
    };
};
} // namespace balkan
