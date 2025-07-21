#include <memory>

#include "VkBootstrap.h"
#include "baleine_type/primitive.h"
#include "baleine_vulkan/RenderState.h"
#include "baleine_vulkan/macros/check.h"
#include "baleine_vulkan/vk_shared/vk_initializers.h"

balkan::SurfaceState::SurfaceState(
    u32 width,
    u32 height,
    VkSurfaceKHR surface,
    Shared<RenderState>&& render_state
) :
    surface(surface),
    render_state(render_state),
    current_swapchain_index(0) {
    for (auto& frame : frames) {
        frame = std::make_unique<FrameData>();
    }

    // Init swapchain
    create_swapchain(width, height, ImageFormat::R8G8B8A8Unorm);

    // Init command pool and buffer
    VkCommandPoolCreateInfo command_pool_create_info =
        vkinit::command_pool_create_info(
            render_state.queue_family,
            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
        );

    for (auto& frame : frames) {
        auto command_pool =
            render_state->device->create_command_pool(command_pool_create_info);

        VkCommandBufferAllocateInfo cmd_alloc_info =
            vkinit::command_buffer_allocate_info(frame->command_pool, 1);

        VkCommandBuffer buffer;
        auto command_buffer =
            command_pool->allocate_command_buffers(cmd_alloc_info);
        frame->command_buffer = std::move(command_buffer);
    }

    // Init sync structures
    auto fence_create_info =
        vkinit::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);

    auto semaphore_create_info = vkinit::semaphore_create_info();

    for (auto& frame : frames) {
        VK_CHECK(vkCreateFence(
            device,
            &fence_create_info,
            nullptr,
            &frame->render_fence
        ));
        VK_CHECK(vkCreateSemaphore(
            device,
            &semaphore_create_info,
            nullptr,
            &frame->swapchain_semaphore
        ));
        VK_CHECK(vkCreateSemaphore(
            device,
            &semaphore_create_info,
            nullptr,
            &frame->render_semaphore
        ));
    }
}

balkan::SurfaceState::~SurfaceState() {
    auto vk_device = render_state->device->vk_device;
    for (auto& frame : frames) {
        vkDestroyCommandPool(
            vk_device,
            frame->command_pool->vk_command_pool,
            nullptr
        );
        vkDestroyFence(vk_device, frame->render_fence, nullptr);
        vkDestroySemaphore(vk_device, frame->render_semaphore, nullptr);
        vkDestroySemaphore(vk_device, frame->swapchain_semaphore, nullptr);
    }
    vkDestroySwapchainKHR(vk_device, swapchain, nullptr);
    vkDestroySurfaceKHR(
        render_state->instance->get_vulkan_instance(),
        surface,
        nullptr
    );
}

void balkan::SurfaceState::create_swapchain(
    u32 width,
    u32 height,
    ImageFormat format
) {
    swapchain_format = static_cast<VkFormat>(format);
    vkb::SwapchainBuilder swapchain_builder {
        render_state->physical_device,
        render_state->device->vk_device,
        surface
    };

    auto vkb_swapchain =
        swapchain_builder
            .set_desired_format(
                VkSurfaceFormatKHR {
                    .format = swapchain_format,
                    .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
                }
            )
            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
            .set_desired_extent(width, height)
            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
            .build()
            .value();

    swapchain_extent = vkb_swapchain.extent;
    swapchain = vkb_swapchain.swapchain;

    Vec<Shared<Image>> images {};
    Vec<Shared<ImageView>> image_views {};

    auto vk_images = vkb_swapchain.get_images().value();
    auto vk_image_views = vkb_swapchain.get_image_views().value();

    for (int i = 0; i < vkb_swapchain.image_count; i++) {
        auto image = std::make_shared<Image>(
            vk_images[i],
            static_cast<ImageFormat>(vkb_swapchain.image_format),
            VkExtent3D {
                vkb_swapchain.extent.width,
                vkb_swapchain.extent.height,
                1
            },
            render_state->device
        );
        images.push_back(image);
        image_views.push_back(
            std::make_shared<ImageView>(vk_image_views[i], image)
        );
    }

    swapchain_images = images;
    swapchain_image_views = image_views;
}

balkan::CommandBuffer& balkan::SurfaceState::reset_and_begin_command() {
    auto& command_buffer = *get_current_frame().command_buffer;
    command_buffer.reset();
    command_buffer.begin();
    return command_buffer;
}

void balkan::SurfaceState::present() {
    const VkPresentInfoKHR present_info {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &get_current_frame().render_semaphore,
        .swapchainCount = 1,
        .pSwapchains = &swapchain,
        .pImageIndices = &current_swapchain_index,
    };

    VK_CHECK(vkQueuePresentKHR(render_state.queue, &present_info));
}

void balkan::SurfaceState::wait_for_current_fences(const u32 timeout) {
    vkWaitForFences(
        render_state->device->vk_device,
        1,
        &get_current_frame().render_fence,
        true,
        timeout
    );
}

void balkan::SurfaceState::reset_current_fences() {
    vkResetFences(render_state.device, 1, &get_current_frame().render_fence);
}

void balkan::SurfaceState::tick_frame_number() {
    frame_number += 1;
}

u32 balkan::SurfaceState::next_swapchain_index() {
    VK_CHECK(vkAcquireNextImageKHR(
        render_state.device,
        swapchain,
        1000000000,
        get_current_frame().swapchain_semaphore,
        nullptr,
        &current_swapchain_index
    ));
    return current_swapchain_index;
}

Shared<balkan::Image> balkan::SurfaceState::get_current_swapchain_image() {
    return swapchain_images[current_swapchain_index];
}

Shared<balkan::ImageView>
balkan::SurfaceState::get_current_swapchain_image_view() {
    return swapchain_image_views[current_swapchain_index];
}

void balkan::SurfaceState::submit_command(const CommandBuffer& cmd) {
    auto cmd_info = vkinit::command_buffer_submit_info(cmd.vk_command_buffer);
    auto wait_info = vkinit::semaphore_submit_info(
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
        get_current_frame().swapchain_semaphore
    );
    auto signal_info = vkinit::semaphore_submit_info(
        VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
        get_current_frame().render_semaphore
    );
    const auto submit =
        vkinit::submit_info(&cmd_info, &signal_info, &wait_info);

    VK_CHECK(vkQueueSubmit2(
        render_state.queue,
        1,
        &submit,
        get_current_frame().render_fence
    ));
}
