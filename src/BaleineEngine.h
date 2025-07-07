#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <span>
#include <array>
#include <functional>
#include <deque>

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vk_mem_alloc.h>

#include <fmt/core.h>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#define STB_IMAGE_IMPLEMENTATION

#define VK_CHECK(x)                                                     \
    do {                                                                \
        VkResult err = x;                                               \
        if (err) {                                                      \
         fmt::print("Detected Vulkan error: {}", string_VkResult(err)); \
            abort();                                                    \
        }                                                               \
} while (0)

class BaleineEngine {
public:
    static BaleineEngine& get();

    bool is_initialized { false };
    int frame_number { 0 };
    bool is_stop_rendering { false };
    VkExtent2D window_extent { 1600, 900 };

    struct SDL_Window* window { nullptr };

    void init();

    void run();

    void draw();

    void cleanup();
};
