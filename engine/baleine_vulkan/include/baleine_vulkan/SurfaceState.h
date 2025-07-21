#pragma once

#include "CommandBuffer.h"
#include "Image.h"
#include "baleine_type/memory.h"
#include "baleine_type/primitive.h"
#include "baleine_type/vector.h"

namespace balkan {
    class RenderState;

    constexpr u32 FRAME_OVERLAP = 2;

    struct FrameData {
        Shared<CommandPool> command_pool;
        Shared<CommandBuffer> command_buffer = nullptr;

        VkSemaphore swapchain_semaphore, render_semaphore;
        VkFence render_fence;
    };

    class SurfaceState : EnableSharedFromThis<SurfaceState>{
    private:
        // 软件需要保证 RenderState 的生命周期 SurfaceState 长
        Shared<RenderState> render_state;

        VkSurfaceKHR surface;

        VkSwapchainKHR swapchain;
        Vec<Shared<Image>> swapchain_images;
        Vec<Shared<ImageView>> swapchain_image_views;
        VkFormat swapchain_format;
        VkExtent2D swapchain_extent;

        u32 current_swapchain_index;

        Unique<FrameData> frames[FRAME_OVERLAP] {};

        u32 frame_number = 0;

        [[nodiscard]] FrameData& get_current_frame() const {
            return *frames[frame_number % FRAME_OVERLAP];
        }

    public:
        explicit SurfaceState(
            u32 width,
            u32 height,
            VkSurfaceKHR surface,
            Shared<RenderState>&& render_state
        );
        ~SurfaceState();

        void create_swapchain(u32 width, u32 height, ImageFormat format);

        CommandBuffer& reset_and_begin_command();
        void submit_command(const CommandBuffer& buffer);
        void present();

        void wait_for_current_fences(u32 timeout = 1000000000);
        void reset_current_fences();

        void begin_frame() {
            wait_for_current_fences();
            reset_current_fences();
            next_swapchain_index();
        }

        void tick_frame_number();
        [[nodiscard]] u32 get_frame_number() const {
            return frame_number;
        }

        u32 next_swapchain_index();

        Shared<Image> get_current_swapchain_image();
        Shared<ImageView> get_current_swapchain_image_view();
    };
}
