#pragma once

#include "CommandPool.h"
#include "FenceSemaphore.h"
#include "Image.h"
#include "baleine_type/memory.h"
#include "vulkan/vulkan.h"

namespace balkan {

struct ImageCreateInfo {
    ImageFormat format;
    ImageUsage usages;
    VkExtent3D extent;
};

enum class CommandPoolCreateFlag : u32 {
    Transient = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
    ResetCommandBuffer = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    Protected = VK_BUFFER_CREATE_PROTECTED_BIT,
};

ENABLE_BITMASK_OPERATORS(CommandPoolCreateFlag);

struct CommandPoolCreateInfo {
    CommandPoolCreateFlag flags;
    u32 queue_family_index{};

    VkCommandPoolCreateInfo vk_info{};

    explicit CommandPoolCreateInfo(CommandPoolCreateFlag flags, u32 queue_family_index);
};


class Device: EnableSharedFromThis<Device> {
  private:
    VmaAllocator allocator;

    friend class SurfaceState;

  public:
    VkDevice vk_device;
    explicit Device(VkDevice vk_device, VmaAllocator allocator);
    ~Device();

    Shared<CommandPool> create_command_pool(CommandPoolCreateInfo& info);
    Shared<Image> create_image(ImageCreateInfo& info);
    Shared<Fence> create_fence(bool signaled);
    Shared<Semaphore> create_semaphore();

    void wait_idle() const;
};
} // namespace balkan