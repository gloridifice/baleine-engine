//
// Created by yifanlin on 2025/7/14.
//

#define VMA_IMPLEMENTATION

#include "Renderer.h"

#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL.h>

#include "VkBootstrap.h"
#include "vk_shared/vk_initializers.h"
#include "vk_shared/vk_utils.h"

constexpr bool USE_VALIDATION_LAYERS = true;

void Renderer::init(SDL_Window& window, u32 width, u32 height) {
    init_vulkan(window);
    init_swapchain(width, height);
    init_commands();
    init_sync_structures();
}

void Renderer::init_vulkan(SDL_Window& window) {
    vkb::InstanceBuilder builder;

    auto instance_ret = builder.set_app_name("").request_validation_layers(USE_VALIDATION_LAYERS).
                                use_default_debug_messenger().require_api_version(1, 3, 0).build();

    vkb::Instance vkb_instance = instance_ret.value();

    instance = vkb_instance.instance;
    debug_messenger = vkb_instance.debug_messenger;

    SDL_Vulkan_CreateSurface(&window, instance, nullptr, &surface);

    //vulkan 1.3 features
    VkPhysicalDeviceVulkan13Features features{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
    features.dynamicRendering = true;
    features.synchronization2 = true;

    //vulkan 1.2 features
    VkPhysicalDeviceVulkan12Features features12{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
    features12.bufferDeviceAddress = true;
    features12.descriptorIndexing = true;

    vkb::PhysicalDeviceSelector selector{vkb_instance};
    auto physical_device = selector.set_minimum_version(1, 3).set_required_features_13(features).
                                    set_required_features_12(features12).set_surface(surface).select().value();

    vkb::DeviceBuilder device_builder{physical_device};
    vkb::Device vkb_device = device_builder.build().value();

    device = vkb_device.device;
    chosen_gpu = physical_device.physical_device;

    queue = vkb_device.get_queue(vkb::QueueType::graphics).value();
    queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();

    // Initialize the memory allocator
    VmaAllocatorCreateInfo allocator_create_info {};
    allocator_create_info.physicalDevice = chosen_gpu;
    allocator_create_info.device = device;
    allocator_create_info.instance = instance;
    allocator_create_info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    vmaCreateAllocator(&allocator_create_info, &allocator);

    main_deletion_queue.push_function([&] { vmaDestroyAllocator(allocator); });
}

void Renderer::init_swapchain(u32 width, u32 height) {
    create_swapchain(width, height);
}

void Renderer::init_commands() {
    VkCommandPoolCreateInfo command_pool_create_info =
        vkinit::command_pool_create_info(queue_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    for (auto& frame : frames) {
        VK_CHECK(vkCreateCommandPool(device, &command_pool_create_info, nullptr, &frame.command_pool));

        VkCommandBufferAllocateInfo cmd_alloc_info =
            vkinit::command_buffer_allocate_info(frame.command_pool, 1);

        VK_CHECK(vkAllocateCommandBuffers(device, &cmd_alloc_info, &frame.main_command_buffer));
    }
}

void Renderer::init_sync_structures() {
    auto fence_create_info =
        vkinit::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);

    auto semaphore_create_info =
        vkinit::semaphore_create_info();

    for (int i = 0; i < FRAME_OVERLAP; i++) {
        VK_CHECK(vkCreateFence(device, &fence_create_info, nullptr, &frames[i].render_fence));
        VK_CHECK(vkCreateSemaphore(device, &semaphore_create_info, nullptr, &frames[i].swapchain_semaphore));
        VK_CHECK(vkCreateSemaphore(device, &semaphore_create_info, nullptr, &frames[i].render_semaphore));
    }
}

void Renderer::draw() {
    // Timeout = 1s
    VK_CHECK(vkWaitForFences(device, 1, &get_current_frame().render_fence, true, 1000000000));
    VK_CHECK(vkResetFences(device, 1, &get_current_frame().render_fence));

    u32 swapchain_image_index;
    VK_CHECK(
        vkAcquireNextImageKHR(device, swapchain, 1000000000, get_current_frame().swapchain_semaphore, nullptr, &
            swapchain_image_index));

    VkCommandBuffer cmd = get_current_frame().main_command_buffer;

    VK_CHECK(vkResetCommandBuffer(cmd, 0));

    VkCommandBufferBeginInfo command_buffer_begin_info =
        vkinit::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VK_CHECK(vkBeginCommandBuffer(cmd, &command_buffer_begin_info));

    draw_extent.width = draw_image.extent.width;
    draw_extent.height = draw_image.extent.height;

    vkutils::transition_image(cmd, draw_image.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

    // ===== Draw =====
    draw_background(cmd);
    // ================

    // ----- Copy draw image to swapchain image -----
    vkutils::transition_image(cmd, draw_image.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    vkutils::transition_image(cmd, swapchain_images[swapchain_image_index], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    vkutils::copy_image_to_image(cmd, draw_image.image, swapchain_images[swapchain_image_index], draw_extent, swapchain_extent);

    vkutils::transition_image(cmd, swapchain_images[swapchain_image_index], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    // -----------------------------------------------

    VK_CHECK(vkEndCommandBuffer(cmd));

    auto cmd_info = vkinit::command_buffer_submit_info(cmd);
    auto wait_info = vkinit::semaphore_submit_info(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, get_current_frame().swapchain_semaphore);
    auto signal_info = vkinit::semaphore_submit_info(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, get_current_frame().render_semaphore);
    auto submit = vkinit::submit_info(&cmd_info, &signal_info, &wait_info);

    VK_CHECK(vkQueueSubmit2(queue, 1, &submit, get_current_frame().render_fence));

    VkPresentInfoKHR present_info{
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &get_current_frame().render_semaphore,
        .swapchainCount = 1,
        .pSwapchains = &swapchain,
        .pImageIndices = &swapchain_image_index,
    };

    VK_CHECK(vkQueuePresentKHR(queue, &present_info));

    frame_deletion_queue.flush();

    frame_number++;
}

void Renderer::draw_background(VkCommandBuffer cmd) {
    const float flash = std::abs(std::sin(static_cast<float>(frame_number) / 120.0f));
    const VkClearColorValue clear_color{{0.0f, 0.0f, flash, 1.0f}};

    const auto clear_range = vkinit::image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);

    vkCmdClearColorImage(cmd, draw_image.image,
                         VK_IMAGE_LAYOUT_GENERAL, &clear_color, 1, &clear_range);
}

void Renderer::create_swapchain(u32 width, u32 height) {
    vkb::SwapchainBuilder swapchain_builder{chosen_gpu, device, surface};
    swapchain_image_format = VK_FORMAT_B8G8R8A8_UNORM;

    auto vkb_swapchain = swapchain_builder
                         .set_desired_format(VkSurfaceFormatKHR{
                             .format = swapchain_image_format,
                             .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
                         })
                         .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
                         .set_desired_extent(width, height)
                         .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
                         .build().value();

    swapchain_extent = vkb_swapchain.extent;
    swapchain = vkb_swapchain.swapchain;

    swapchain_images = vkb_swapchain.get_images().value();
    swapchain_image_views = vkb_swapchain.get_image_views().value();

    VkExtent3D extent {
        width,
        height,
        1
    };

    draw_image.format = VK_FORMAT_R16G16B16A16_SFLOAT;
    draw_image.extent = extent;

    VkImageUsageFlags draw_image_flags {};
    draw_image_flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    draw_image_flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    draw_image_flags |= VK_IMAGE_USAGE_STORAGE_BIT;
    draw_image_flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    auto image_create_info = vkinit::image_create_info(draw_image.format, draw_image_flags, extent);

    VmaAllocationCreateInfo allocation_create_info {};
    allocation_create_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocation_create_info.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    vmaCreateImage(allocator, &image_create_info, &allocation_create_info, &draw_image.image, &draw_image.allocation, nullptr);
    auto view_create_info = vkinit::imageview_create_info(draw_image.format, draw_image.image, VK_IMAGE_ASPECT_COLOR_BIT);

    VK_CHECK(vkCreateImageView(device, &view_create_info, nullptr, &draw_image.view));

    main_deletion_queue.push_function([this] {
        vkDestroyImageView(device, draw_image.view, nullptr);
        vmaDestroyImage(allocator, draw_image.image, draw_image.allocation);
    });
}

void Renderer::destroy_swapchain() {
    vkDestroySwapchainKHR(device, swapchain, nullptr);

    for (auto& swapchain_image_view : swapchain_image_views) {
        vkDestroyImageView(device, swapchain_image_view, nullptr);
    }
}

void Renderer::cleanup() {
    vkDeviceWaitIdle(device);

    // Cleanup
    for (auto & frame : frames) {
        vkDestroyCommandPool(device, frame.command_pool, nullptr);
        vkDestroyFence(device, frame.render_fence, nullptr);
        vkDestroySemaphore(device, frames->render_semaphore, nullptr);
        vkDestroySemaphore(device, frames->swapchain_semaphore, nullptr);
    }

    main_deletion_queue.flush();

    destroy_swapchain();
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyDevice(device, nullptr);

    vkb::destroy_debug_utils_messenger(instance, debug_messenger);
    vkDestroyInstance(instance, nullptr);
}
