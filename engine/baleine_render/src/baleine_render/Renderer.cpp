//
// Created by yifanlin on 2025/7/14.
//

#define VMA_IMPLEMENTATION

#include "baleine_render/Renderer.h"

#include <SDL3/SDL_vulkan.h>

#include <cmath>

#include "VkBootstrap.h"

void Renderer::init(SDL_Window& window, u32 width, u32 height) {
    auto instance = std::make_unique<Instance>("My Vulkan App");
    VkSurfaceKHR surface;
    SDL_Vulkan_CreateSurface(&window, instance->get_vulkan_instance(), nullptr, &surface);
    render_state = std::make_unique<RenderState>(std::move(instance), surface);
    surface_state = render_state->create_surface(surface, width, height);
    create_draw_image(width, height);
}

void Renderer::draw() {
    // Timeout = 1s
    surface_state->begin_frame();

    auto& cmd = surface_state->reset_and_begin_command();

    draw_extent.width = draw_image->extent.width;
    draw_extent.height = draw_image->extent.height;

    cmd.transition_image(*draw_image, ImageLayout::General);

    // ===== Draw =====
    const f32 flash = std::abs(std::sin(static_cast<float>(surface_state->get_frame_number()) / 120.0f));
    const VkClearColorValue clear_color{{0.0f, 0.0f, flash, 1.0f}};

    cmd.clear_color_image(*draw_image, clear_color);
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

void Renderer::create_draw_image(u32 width, u32 height) {
    VkExtent3D extent {
        width,
        height,
        1
    };

    draw_image = render_state->device->create_image(ImageCreateInfo {
        ImageFormat::R16G16B16A16Sfloat,
        ImageUsage::TransferDst | ImageUsage::TransferSrc | ImageUsage::Storage | ImageUsage::ColorAttachment,
        extent
    });
}

void Renderer::cleanup() const {
    render_state->device->wait_idle();
}
