#pragma once

#include "baleine_type/memory.h"
#include "vulkan/vulkan.h"

namespace balkan {

class Device;
class CommandBuffer;

class CommandPool: EnableSharedFromThis<CommandPool> {
  private:
    Shared<Device> device;

  public:
    VkCommandPool vk_command_pool;

    explicit CommandPool(VkCommandPool command_pool, Shared<Device>&& device);

    ~CommandPool();

    Shared<CommandBuffer>
    allocate_command_buffers(VkCommandBufferAllocateInfo&& info);

    Device& get_device() {
        return *device;
    }
};
} // namespace balkan
