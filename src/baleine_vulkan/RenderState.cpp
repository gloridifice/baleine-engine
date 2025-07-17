#include "RenderState.h"

#include <SDL3/SDL_vulkan.h>

#include "vk_shared/vk_initializers.h"
#include "VkBootstrap.h"
#include "macros/check.h"

namespace balkan {
    RenderState::RenderState() {
        vkb::Instance vkb_instance;

        //vulkan 1.3 features
        VkPhysicalDeviceVulkan13Features features{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
        features.dynamicRendering = true;
        features.synchronization2 = true;

        //vulkan 1.2 features
        VkPhysicalDeviceVulkan12Features features12{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
        features12.bufferDeviceAddress = true;
        features12.descriptorIndexing = true;

        vkb::PhysicalDeviceSelector selector{vkb_instance};
        auto physical_device_info = selector.set_minimum_version(1, 3).set_required_features_13(features).
                                             set_required_features_12(features12).select().value();

        vkb::DeviceBuilder device_builder{physical_device_info};
        vkb::Device vkb_device = device_builder.build().value();

        instance = vkb_instance.instance;
        device = vkb_device.device;
        physical_device = vkb_device.physical_device;

        queue = vkb_device.get_queue(vkb::QueueType::graphics).value();
        queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();

        // Initialize the memory allocator
        VmaAllocatorCreateInfo allocator_create_info{};
        allocator_create_info.physicalDevice = physical_device;
        allocator_create_info.device = device;
        allocator_create_info.instance = instance;
        allocator_create_info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        VK_CHECK(vmaCreateAllocator(&allocator_create_info, &allocator));
    }

    Shared<Image> RenderState::create_image(ImageCreateInfo&& info) const {
        const auto image = std::make_shared<Image>(nullptr, static_cast<Format>(info.format), info.extent, device);
        const auto image_create_info = vkinit::image_create_info(
            info.format, static_cast<VkImageUsageFlags>(info.usages), info.extent);

        VmaAllocationCreateInfo allocation_create_info{};
        allocation_create_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        allocation_create_info.requiredFlags = static_cast<VkMemoryPropertyFlags>(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        VK_CHECK(vmaCreateImage(allocator, &image_create_info, &allocation_create_info, &image->image, &image->allocation, nullptr));

        return image;
    }

    Shared<SurfaceState> RenderState::create_surface_by_sdl_window(SDL_Window* window, u32 width, u32 height) {
        VkSurfaceKHR surface;
        SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface);
        return std::make_shared<SurfaceState>(width, height, surface, *this);
    }

    RenderState::~RenderState() {
        vmaDestroyAllocator(allocator);
        vkDestroyDevice(device, nullptr);
        vkDestroyInstance(instance, nullptr);
    }
}
