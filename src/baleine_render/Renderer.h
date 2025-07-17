//
// Created by koiro on 2025/7/14.
//

#pragma once

#include <deque>
#include <functional>
#include <string>
#include <fmt/core.h>
#include <vulkan/vulkan.h>

#include "AllocatedImage.h"
#include "../baleine_type/primitive.h"
#include "../baleine_type/vector.h"

struct DescriptorLayoutBuilder {
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    void add_binding(uint32_t binding);
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

class DeletionQueue {
    std::deque<std::function<void()>> deletors;

public:
    void push_function(std::function<void()>&& function) {
        deletors.push_back(function);
    }

    void flush() {
        for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
            (*it)();
        }

        deletors.clear();
    }
};

constexpr uint32_t FRAME_OVERLAP = 2;

class SDL_Window;

class Renderer {
public:
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkPhysicalDevice chosen_gpu;
    VkDevice device;
    VkSurfaceKHR surface;

    // Swapchain
    VkSwapchainKHR swapchain;
    VkFormat swapchain_image_format;

    Vec<VkImage> swapchain_images;
    Vec<VkImageView> swapchain_image_views;
    VkExtent2D swapchain_extent;

    // Frame
    FrameData frames[FRAME_OVERLAP];
    u32 frame_number;
    VkQueue queue;
    u32 queue_family;

    DeletionQueue frame_deletion_queue;
    DeletionQueue main_deletion_queue;

    VmaAllocator allocator;

    // Draw image
    AllocatedImage draw_image;
    VkExtent2D draw_extent;

public:
    void init(SDL_Window& window, u32 width, u32 height);
    void draw();
    void draw_background(VkCommandBuffer cmd);
    void create_swapchain(u32 width, u32 height);
    void cleanup();

    FrameData& get_current_frame() {
        return frames[frame_number % FRAME_OVERLAP];
    }

private:
    void init_vulkan(SDL_Window& window);
    void init_swapchain(u32 width, u32 height);
    void init_commands();
    void init_sync_structures();

    void destroy_swapchain();
};
