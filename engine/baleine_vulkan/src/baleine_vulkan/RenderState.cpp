
#include "baleine_vulkan/RenderState.h"

#include "VkBootstrap.h"
#include "baleine_type/primitive.h"
#include "baleine_vulkan/error.h"
#include "baleine_vulkan/macros/check.h"

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

    queue = vkb_device.get_queue(vkb::QueueType::graphics).value();
    queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();

    // Initialize the memory allocator
    VmaAllocatorCreateInfo allocator_create_info {};
    allocator_create_info.physicalDevice = physical_device;
    allocator_create_info.device = vkb_device.device;
    allocator_create_info.instance = instance->get_vulkan_instance();
    allocator_create_info.flags =
        VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    VK_CHECK(vmaCreateAllocator(&allocator_create_info, &allocator));

    device = std::make_unique<Device>(vkb_device.device, allocator);
}

Shared<SurfaceState>
RenderState::create_surface(VkSurfaceKHR surface, u32 width, u32 height) {
    return std::make_shared<SurfaceState>(width, height, surface, shared_from_this());
}

RenderState::~RenderState() {
    vmaDestroyAllocator(allocator);
}
} // namespace balkan