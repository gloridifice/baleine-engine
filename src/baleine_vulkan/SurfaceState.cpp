#include "SurfaceState.h"

#include <memory>

#include "macros/check.h"
#include "RenderState.h"
#include "VkBootstrap.h"
#include "../baleine_render/vk_shared/vk_initializers.h"

balkan::SurfaceState::SurfaceState(
    u32 width,
    u32 height,
    VkSurfaceKHR surface,
    RenderState& render_state
):
    surface(surface), render_state(render_state) {

    auto device = render_state.device;
    // Init swapchain
    vkb::SwapchainBuilder swapchain_builder{render_state.physical_device, render_state.device, surface};

    swapchain_format = VK_FORMAT_B8G8R8A8_UNORM;

    auto vkb_swapchain = swapchain_builder
                         .set_desired_format(VkSurfaceFormatKHR{
                             .format = swapchain_format,
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

    // Init command pool and buffer
    VkCommandPoolCreateInfo command_pool_create_info =
        vkinit::command_pool_create_info(render_state.queue_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    for (auto& frame : frames) {
        VK_CHECK(vkCreateCommandPool(device, &command_pool_create_info, nullptr, &frame.command_pool));

        VkCommandBufferAllocateInfo cmd_alloc_info =
            vkinit::command_buffer_allocate_info(frame.command_pool, 1);

        VkCommandBuffer buffer;
        VK_CHECK(vkAllocateCommandBuffers(device, &cmd_alloc_info, &buffer));
        frame.command_buffer = std::make_unique<CommandBuffer>(buffer);
    }

    // Init sync structures
    auto fence_create_info =
        vkinit::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);

    auto semaphore_create_info =
        vkinit::semaphore_create_info();

    for (auto& frame : frames) {
        VK_CHECK(vkCreateFence(device, &fence_create_info, nullptr, &frame.render_fence));
        VK_CHECK(vkCreateSemaphore(device, &semaphore_create_info, nullptr, &frame.swapchain_semaphore));
        VK_CHECK(vkCreateSemaphore(device, &semaphore_create_info, nullptr, &frame.render_semaphore));
    }
}

balkan::SurfaceState::~SurfaceState() {
    vkDestroySurfaceKHR(render_state.instance, surface, nullptr);
}

balkan::CommandBuffer& balkan::SurfaceState::begin_command() {
    auto& command_buffer = *get_current_frame().command_buffer;
    command_buffer.reset();
    command_buffer.begin();
    return command_buffer;
}

void balkan::SurfaceState::present() {
    u32 swapchain_image_index;
    VK_CHECK(
        vkAcquireNextImageKHR(render_state.device, swapchain, 1000000000, get_current_frame().swapchain_semaphore,
            nullptr, &
            swapchain_image_index));

    VkPresentInfoKHR present_info{
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &get_current_frame().render_semaphore,
        .swapchainCount = 1,
        .pSwapchains = &swapchain,
        .pImageIndices = &swapchain_image_index,
    };

    vkQueuePresentKHR(render_state.queue, &present_info);
}

void balkan::SurfaceState::submit_command(const CommandBuffer& cmd) {
    auto cmd_info = vkinit::command_buffer_submit_info(cmd.cmd);
    auto wait_info = vkinit::semaphore_submit_info(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
                                                   get_current_frame().swapchain_semaphore);
    auto signal_info = vkinit::semaphore_submit_info(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
                                                     get_current_frame().render_semaphore);
    const auto submit = vkinit::submit_info(&cmd_info, &signal_info, &wait_info);

    VK_CHECK(vkQueueSubmit2(render_state.queue, 1, &submit, get_current_frame().render_fence));
}
