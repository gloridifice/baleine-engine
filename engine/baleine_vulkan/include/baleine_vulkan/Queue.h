#pragma once

#include "baleine_type/memory.h"
#include "baleine_type/primitive.h"
#include "vulkan/vulkan.h"

namespace balkan {
class Queue: EnableSharedFromThis<Queue> {
    VkQueue vk_queue;
    u32 queue_family;

  public:
    explicit Queue(VkQueue queue);
    ~Queue();
};
} // namespace balkan