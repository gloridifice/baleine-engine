//
// Created by koiro on 2025/7/14.
//

#pragma once

#include <vector>
#include <string>
#include <fmt/core.h>
#include <vulkan/vulkan.h>

#define VK_CHECK(x)                                                     \
do {                                                                \
VkResult err = x;                                               \
if (err) {                                                      \
fmt::print("Detected Vulkan error: {}", err); \
abort();                                                    \
}                                                               \
} while (0)

template <>
struct fmt::formatter<VkResult> : fmt::formatter<std::string_view> {
    auto format(VkResult result, format_context& ctx) const {
        const char* str = "Unknown VkResult";
        switch (result) {
        case VK_SUCCESS: str = "VK_SUCCESS"; break;
        case VK_NOT_READY: str = "VK_NOT_READY"; break;
        case VK_TIMEOUT: str = "VK_TIMEOUT"; break;
        case VK_EVENT_SET: str = "VK_EVENT_SET"; break;
        case VK_EVENT_RESET: str = "VK_EVENT_RESET"; break;
        case VK_INCOMPLETE: str = "VK_INCOMPLETE"; break;
        case VK_ERROR_OUT_OF_HOST_MEMORY: str = "VK_ERROR_OUT_OF_HOST_MEMORY"; break;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY: str = "VK_ERROR_OUT_OF_DEVICE_MEMORY"; break;
            // 添加更多 VkResult 枚举值...
        default: break;
        }
        return fmt::formatter<std::string_view>::format(str, ctx);
    }
};

struct FrameData {
    VkCommandPool command_pool;
    VkCommandBuffer main_command_buffer;

    /// Semaphore 是用于指定 GPU 中的两个命令的先后顺序；
    /// 若不指定，命令可能被并行执行。
    VkSemaphore swapchain_semaphore, render_semaphore;

    /// Fence 是用于 CPU 与 GPU 之间通信任务等待的工具；
    /// 提交命令后可以获得 Fence，CPU 可以等待该 Fence 执行完毕。
    VkFence render_fence;
};

constexpr uint32_t FRAME_OVERLAP = 2;

class SDL_Window;

class RenderState {
public:
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkPhysicalDevice chosen_gpu;
    VkDevice device;
    VkSurfaceKHR surface;

    // Swapchain
    VkSwapchainKHR swapchain;
    VkFormat swapchain_image_format;

    std::vector<VkImage> swapchain_images;
    std::vector<VkImageView> swapchain_image_views;
    VkExtent2D swapchain_extent;

    // Frame
    FrameData frames[FRAME_OVERLAP];
    uint32_t frame_number;
    VkQueue queue;
    uint32_t queue_family;

public:
    void init(SDL_Window& window, uint32_t width, uint32_t height);
    void draw();
    void create_swapchain(uint32_t width, uint32_t height);
    void cleanup();

    FrameData& get_current_frame() {
        return frames[frame_number % FRAME_OVERLAP];
    }

private:
    void init_vulkan(SDL_Window& window);
    void init_swapchain(uint32_t width, uint32_t height);
    void init_commands();
    void init_sync_structures();

    void destroy_swapchain();
};
