#include "baleine_vulkan/Device.h"

#include "baleine_vulkan/macros/check.h"
#include "baleine_vulkan/vk_shared/vk_initializers.h"

balkan::Device::Device(VkDevice vk_device, VmaAllocator allocator) :
    vk_device(vk_device),
    allocator(allocator) {}

balkan::Device::~Device() {
    vkDestroyDevice(vk_device, nullptr);
}

Shared<balkan::CommandPool>
balkan::Device::create_command_pool(VkCommandPoolCreateInfo&& info) {
    VkCommandPool command_pool;
    vkCreateCommandPool(vk_device, &info, nullptr, &command_pool);
    return std::make_shared<CommandPool>(command_pool, shared_from_this());
}

Shared<balkan::Image>
balkan::Device::create_image(ImageCreateInfo&& info) {
    auto image =
        std::make_shared<Image>(nullptr, info.format, info.extent, shared_from_this());
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

void balkan::Device::wait_idle() const {
    vkDeviceWaitIdle(vk_device);
}