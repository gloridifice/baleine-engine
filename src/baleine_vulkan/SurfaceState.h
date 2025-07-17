#pragma once

#include <vulkan/vulkan.h>

#include "CommandBuffer.h"
#include "Image.h"
#include "../baleine_type/functional.h"
#include "../baleine_type/memory.h"
#include "../baleine_type/primitive.h"
#include "../baleine_type/vector.h"

namespace balkan {
    class RenderState;

    constexpr u32 FRAME_OVERLAP = 2;

    struct FrameData {
        VkCommandPool command_pool;
        Unique<CommandBuffer> command_buffer;

        VkSemaphore swapchain_semaphore, render_semaphore;
        VkFence render_fence;
    };

    class SurfaceState {

    private:
        // 软件需要保证 RenderState 的生命周期 SurfaceState 长
        RenderState& render_state;

        VkSurfaceKHR surface;

        VkSwapchainKHR swapchain;
        Vec<VkImage> swapchain_images;
        Vec<VkImageView> swapchain_image_views;
        VkFormat swapchain_format;
        VkExtent2D swapchain_extent;

        FrameData frames[FRAME_OVERLAP];

        u32 frame_number = 0;

        FrameData& get_current_frame() {
            return frames[frame_number % FRAME_OVERLAP];
        }

    public:
        explicit SurfaceState(
            u32 width,
            u32 height,
            VkSurfaceKHR surface,
            RenderState& render_state
        );
        ~SurfaceState();

        CommandBuffer& begin_command();
        void submit_command(const CommandBuffer& buffer);
        void present();
    };
}
