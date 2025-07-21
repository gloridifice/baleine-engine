
#include "baleine_vulkan/RenderState.h"

#include "baleine_vulkan/error.h"
#include "baleine_vulkan/macros/check.h"
#include "VkBootstrap.h"
#include "baleine_vulkan/vk_shared/vk_initializers.h"
#include "baleine_type/primitive.h"

namespace balkan {
RenderState::RenderState(
    Unique<Instance>&& moved_instance,
    VkSurfaceKHR primary_surface
) :
    allocator(nullptr) {
    instance = std::move(moved_instance);

    // ===== Select Physical Device =====
    //vulkan 1.3 features
    VkPhysicalDeviceVulkan13Features features {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES
    };
    features.dynamicRendering = true;
    features.synchronization2 = true;

    //vulkan 1.2 features
    VkPhysicalDeviceVulkan12Features features12 {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES
    };
    features12.bufferDeviceAddress = true;
    features12.descriptorIndexing = true;

    vkb::PhysicalDeviceSelector selector {instance->vkb_instance};
    auto physical_device_info_result = selector.set_minimum_version(1, 3)
                                           .set_required_features_13(features)
                                           .set_required_features_12(features12)
                                           .set_surface(primary_surface)
                                           .select();

    if (!physical_device_info_result)
        throw CreationException(
            "Physical device",
            physical_device_info_result.error().message()
        );
    const auto& physical_device_info = physical_device_info_result.value();

    // ===== Device =====
    vkb::DeviceBuilder device_builder {physical_device_info};
    vkb::Device vkb_device = device_builder.build().value();

    physical_device = vkb_device.physical_device;
    device = vkb_device.device;

    queue = vkb_device.get_queue(vkb::QueueType::graphics).value();
    queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();

    // Initialize the memory allocator
    VmaAllocatorCreateInfo allocator_create_info {};
    allocator_create_info.physicalDevice = physical_device;
    allocator_create_info.device = device;
    allocator_create_info.instance = instance->get_vulkan_instance();
    allocator_create_info.flags =
        VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    VK_CHECK(vmaCreateAllocator(&allocator_create_info, &allocator));
}

Shared<Image> RenderState::create_image(ImageCreateInfo&& info) const {
    const auto image =
        std::make_shared<Image>(nullptr, info.format, info.extent, device);
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

    return image;
}

Shared<SurfaceState>
RenderState::create_surface(VkSurfaceKHR surface, u32 width, u32 height) {
    return std::make_shared<SurfaceState>(width, height, surface, *this);
}

void RenderState::device_wait_idle() const {
    vkDeviceWaitIdle(device);
}

RenderState::~RenderState() {
    vmaDestroyAllocator(allocator);
    vkDestroyDevice(device, nullptr);
}
} // namespace balkan