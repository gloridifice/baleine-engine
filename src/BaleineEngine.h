#pragma once

#include <memory>

#include <vulkan/vulkan.h>

#define STB_IMAGE_IMPLEMENTATION

class Renderer;

class BaleineEngine {
public:
    static BaleineEngine& get();

    bool is_initialized { false };
    int frame_number { 0 };
    bool is_stop_rendering { false };
    VkExtent2D window_extent { 1600, 900 };

    struct SDL_Window* window { nullptr };
    std::unique_ptr<Renderer> render_state;

    BaleineEngine();
    ~BaleineEngine();

    void init();

    void run();

    void draw();

    void cleanup() const;
};
