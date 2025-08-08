#pragma once

#include <vulkan/vulkan.hpp>

#define STB_IMAGE_IMPLEMENTATION

class BaleineEngine {
public:
    static BaleineEngine& get();

    bool is_initialized { false };
    int frame_number { 0 };
    bool is_stop_rendering { false };
    vk::Extent2D window_extent { 1600, 900 };

    struct SDL_Window* window { nullptr };

    BaleineEngine();
    ~BaleineEngine();

    void init();

    void run();

    void draw();

    void cleanup() const;
};
