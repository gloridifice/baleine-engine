#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <SDL3/SDL.h>

#include "Image.h"
#include "SurfaceState.h"
#include "../baleine_type/memory.h"
#include "../baleine_type/primitive.h"

namespace balkan {
    struct ImageCreateInfo {
        VkFormat format;
        ImageUsage usages;
        VkExtent3D extent;
    };

    class RenderState {
    public:
        VkInstance instance;
        VkDevice device;
        VkPhysicalDevice physical_device;
        VmaAllocator allocator;
        VkQueue queue;

        u32 queue_family;

        RenderState();
        ~RenderState();

        Shared<Image> create_image(ImageCreateInfo&& info) const;
        Shared<SurfaceState> create_surface_by_sdl_window(SDL_Window* window, u32 width, u32 height);
    };
}
