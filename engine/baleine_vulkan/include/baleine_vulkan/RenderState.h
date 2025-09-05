#pragma once

#include <vk_mem_alloc.h>

#include "Device.h"
#include "Image.h"
#include "Instance.h"
#include "SurfaceState.h"
#include "baleine_type/memory.h"
#include "baleine_type/primitive.h"

namespace balkan {
class RenderState : EnableSharedFromThis<RenderState>{
  public:
    Shared<Instance> instance;
    Shared<Device> device;
    VkPhysicalDevice physical_device;
    VmaAllocator allocator;
    VkQueue queue;

    u32 queue_family;

    RenderState(Unique<Instance>&& moved_instance, VkSurfaceKHR primary_surface);
    ~RenderState();

    auto create_surface(VkSurfaceKHR surface, u32 width, u32 height)
        -> Shared<SurfaceState>;
};
} // namespace balkan
