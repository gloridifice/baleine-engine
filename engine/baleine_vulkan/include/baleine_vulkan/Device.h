#pragma once

#include "CommandPool.h"
#include "Image.h"
#include "baleine_type/memory.h"
#include "vulkan/vulkan.h"

namespace balkan {
struct ImageCreateInfo {
    ImageFormat format;
    ImageUsage usages;
    VkExtent3D extent;
};

class Device: EnableSharedFromThis<Device> {
  private:
    VmaAllocator allocator;

    friend class SurfaceState;

  public:
    VkDevice vk_device;
    explicit Device(VkDevice vk_device, VmaAllocator allocator);
    ~Device();

    Shared<CommandPool> create_command_pool(VkCommandPoolCreateInfo&& info);
    Shared<Image> create_image(ImageCreateInfo&& info);

    void wait_idle() const;
};
} // namespace balkan