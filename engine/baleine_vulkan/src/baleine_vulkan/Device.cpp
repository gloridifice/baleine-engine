#include "baleine_vulkan/Device.h"

#include "baleine_type/functional.h"
#include "baleine_type/vector.h"
#include "baleine_vulkan/macros/check.h"
#include "baleine_vulkan/vk_shared/vk_initializers.h"

balkan::CommandPoolCreateInfo::CommandPoolCreateInfo(
    CommandPoolCreateFlag flags,
    u32 queue_family_index
) :flags(flags), queue_family_index(queue_family_index){
    vk_info = VkCommandPoolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = static_cast<VkCommandPoolCreateFlags>(flags),
        .queueFamilyIndex = queue_family_index
    };
}

balkan::Device::Device(VkDevice vk_device, VmaAllocator allocator) :
    vk_device(vk_device),
    allocator(allocator) {}

balkan::Device::~Device() {
    vkDestroyDevice(vk_device, nullptr);
}

Shared<balkan::CommandPool>
balkan::Device::create_command_pool(CommandPoolCreateInfo& info) {
    VkCommandPool command_pool;
    vkCreateCommandPool(vk_device, &info.vk_info, nullptr, &command_pool);
    return std::make_shared<CommandPool>(command_pool, shared_from_this());
}

Shared<balkan::Image> balkan::Device::create_image(ImageCreateInfo& info) {
    auto image = std::make_shared<Image>(
        nullptr,
        info.format,
        info.extent,
        shared_from_this()
    );

    const auto image_create_info = vkinit::image_create_info(
        static_cast<VkFormat>(info.format),
        static_cast<VkImageUsageFlags>(info.usages),
        info.extent
    );

    VmaAllocationCreateInfo allocation_create_info {};
    allocation_create_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocation_create_info.requiredFlags =
        static_cast<VkMemoryPropertyFlags>(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VK_CHECK(vmaCreateImage(
        allocator,
        &image_create_info,
        &allocation_create_info,
        &image->image,
        &image->allocation,
        nullptr
    ));

    return std::move(image);
}

Shared<balkan::Fence> balkan::Device::create_fence(const bool signaled) {
    const auto info =
        vkinit::fence_create_info(signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0);
    VkFence fence;
    vkCreateFence(vk_device, &info, nullptr, &fence);
    return std::make_shared<Fence>(fence, shared_from_this());
}

Shared<balkan::Semaphore> balkan::Device::create_semaphore() {
    const auto info = vkinit::semaphore_create_info();
    VkSemaphore semaphore;
    vkCreateSemaphore(vk_device, &info, nullptr, &semaphore);
    return std::make_shared<Semaphore>(semaphore, shared_from_this());
}

void balkan::Device::wait_idle() const {
    vkDeviceWaitIdle(vk_device);
}