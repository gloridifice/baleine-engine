//
// Created by yifanlin on 2025/7/14.
//

#define VMA_IMPLEMENTATION

#include "Renderer.h"

#include <SDL3/SDL_vulkan.h>

#include "VkBootstrap.h"

void Renderer::init(SDL_Window& window, u32 width, u32 height) {
    render_state = std::make_unique<RenderState>();
    surface_state = render_state->create_surface_by_sdl_window(&window, width, height);
}

void Renderer::draw() {
    // Timeout = 1s
    surface_state->wait_for_current_fences();
    surface_state->reset_current_fences();

    auto& cmd = surface_state->reset_and_begin_command();

    draw_extent.width = draw_image->extent.width;
    draw_extent.height = draw_image->extent.height;

    cmd.transition_image(*draw_image, ImageLayout::General);

    // ===== Draw =====
    draw_background(cmd);
    // ================

    // ----- Copy draw image to swapchain image -----

    auto& current_swapchain_image = *surface_state->get_current_swapchain_image();

    auto extent = surface_state->get_current_swapchain_image()->extent;
    cmd.copy_image_to_image(*draw_image, current_swapchain_image, draw_extent, extent);

    cmd.transition_image(current_swapchain_image, ImageLayout::PresentSrcKHR);
    // -----------------------------------------------

    cmd.end();
    surface_state->submit_command(cmd);
    surface_state->present();

    surface_state->tick_frame_number();
}

void Renderer::draw_background(VkCommandBuffer cmd) {
    const float flash = std::abs(std::sin(static_cast<float>() / 120.0f));
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
