
#include "baleine_vulkan/CommandPool.h"

#include "baleine_vulkan/CommandBuffer.h"
#include "baleine_vulkan/Device.h"

namespace balkan {
CommandPool::CommandPool(VkCommandPool command_pool, Shared<Device>&& device) :
    vk_command_pool(command_pool),
    device(device) {}

CommandPool::~CommandPool() {
    vkDestroyCommandPool(device->vk_device, vk_command_pool, nullptr);
}

Shared<CommandBuffer>
CommandPool::allocate_command_buffers(VkCommandBufferAllocateInfo&& info) {
    VkCommandBuffer buffer;
    vkAllocateCommandBuffers(device->vk_device, &info, &buffer);
    return std::make_shared<CommandBuffer>(buffer, shared_from_this());
}

} // namespace balkan