//
// Created by yifanlin on 2025/7/14.
//

#include "RenderState.h"

#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL.h>

#include "VkBootstrap.h"
#include "vk_shared/vk_initializers.h"
#include "vk_shared/vk_utils.h"

constexpr bool USE_VALIDATION_LAYERS = true;

void RenderState::init(SDL_Window& window, uint32_t width, uint32_t height) {
    init_vulkan(window);
    init_swapchain(width, height);
    init_commands();
    init_sync_structures();
}

void RenderState::init_vulkan(SDL_Window& window) {
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
}

void RenderState::init_swapchain(uint32_t width, uint32_t height) {
    create_swapchain(width, height);
}

void RenderState::init_commands() {
    VkCommandPoolCreateInfo command_pool_create_info =
        vkinit::command_pool_create_info(queue_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    for (auto& frame : frames) {
        VK_CHECK(vkCreateCommandPool(device, &command_pool_create_info, nullptr, &frame.command_pool));

        VkCommandBufferAllocateInfo cmd_alloc_info =
            vkinit::command_buffer_allocate_info(frame.command_pool, 1);

        VK_CHECK(vkAllocateCommandBuffers(device, &cmd_alloc_info, &frame.main_command_buffer));
    }
}

void RenderState::init_sync_structures() {
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

void RenderState::draw() {
    // Timeout = 1s
    VK_CHECK(vkWaitForFences(device, 1, &get_current_frame().render_fence, true, 1000000000));
    VK_CHECK(vkResetFences(device, 1, &get_current_frame().render_fence));

    uint32_t swapchain_image_index;
    VK_CHECK(
        vkAcquireNextImageKHR(device, swapchain, 1000000000, get_current_frame().swapchain_semaphore, nullptr, &
            swapchain_image_index));

    VkCommandBuffer cmd = get_current_frame().main_command_buffer;

    VK_CHECK(vkResetCommandBuffer(cmd, 0));

    VkCommandBufferBeginInfo command_buffer_begin_info =
        vkinit::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VK_CHECK(vkBeginCommandBuffer(cmd, &command_buffer_begin_info));

    auto& current_image = swapchain_images[swapchain_image_index];

    vkutils::transition_image(cmd, current_image,
                              VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
    const float flash = std::abs(std::sin(static_cast<float>(frame_number) / 120.0f));
    const VkClearColorValue clear_color{{0.0f, 0.0f, flash, 1.0f}};

    const auto clear_range = vkinit::image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);

    vkCmdClearColorImage(cmd, current_image,
                         VK_IMAGE_LAYOUT_GENERAL, &clear_color, 1, &clear_range);

    vkutils::transition_image(cmd, current_image,
                              VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

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

    frame_number++;
}

void RenderState::create_swapchain(uint32_t width, uint32_t height) {
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
}

void RenderState::destroy_swapchain() {
    vkDestroySwapchainKHR(device, swapchain, nullptr);

    for (auto& swapchain_image_view : swapchain_image_views) {
        vkDestroyImageView(device, swapchain_image_view, nullptr);
    }
}

void RenderState::cleanup() {
    vkDeviceWaitIdle(device);

    // Cleanup
    for (auto & frame : frames) {
        vkDestroyCommandPool(device, frame.command_pool, nullptr);
        vkDestroyFence(device, frame.render_fence, nullptr);
        vkDestroySemaphore(device, frames->render_semaphore, nullptr);
        vkDestroySemaphore(device, frames->swapchain_semaphore, nullptr);
    }

    for (auto& frame : frames) {
        vkDestroyCommandPool(device, frame.command_pool, nullptr);
    }

    destroy_swapchain();
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyDevice(device, nullptr);

    vkb::destroy_debug_utils_messenger(instance, debug_messenger);
    vkDestroyInstance(instance, nullptr);
}
