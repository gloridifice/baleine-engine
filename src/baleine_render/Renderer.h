//
// Created by koiro on 2025/7/14.
//

#pragma once

#include <deque>
#include <functional>
#include <string>
#include <vulkan/vulkan.h>

#include "../baleine_type/memory.h"
#include "../baleine_type/primitive.h"
#include "../baleine_vulkan/RenderState.h"


using namespace balkan;

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

class SDL_Window;

class Renderer {
public:
    VkDebugUtilsMessengerEXT debug_messenger;
    Unique<RenderState> render_state;
    Shared<SurfaceState> surface_state;
    SDL_Window* window;

    Shared<Image> draw_image;
    VkExtent3D draw_extent;

public:
    void init(SDL_Window& window, u32 width, u32 height);
    void draw();
    void create_draw_image(u32 width, u32 height);
    void cleanup();
};
