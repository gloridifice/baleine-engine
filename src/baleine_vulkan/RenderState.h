#pragma once

#include <SDL3/SDL.h>
#include <vk_mem_alloc.h>

#include "../baleine_type/memory.h"
#include "../baleine_type/primitive.h"
#include "Image.h"
#include "Instance.h"
#include "SurfaceState.h"

namespace balkan {
struct ImageCreateInfo {
    ImageFormat format;
    ImageUsage usages;
    VkExtent3D extent;
};

class RenderState {
  public:
    Unique<Instance> instance;
    VkDevice device;
    VkPhysicalDevice physical_device;
    VmaAllocator allocator;
    VkQueue queue;

    u32 queue_family;

    RenderState(Unique<Instance>&& moved_instance, VkSurfaceKHR primary_surface);
    ~RenderState();

    auto create_image(ImageCreateInfo&& info) const -> Shared<Image>;
    auto create_surface(VkSurfaceKHR surface, u32 width, u32 height)
        -> Shared<SurfaceState>;

    void device_wait_idle() const;
};
} // namespace balkan
