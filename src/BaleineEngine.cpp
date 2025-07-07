//
// Created by yifanlin on 2025/7/7.
//

#include "BaleineEngine.h"

#include <bits/this_thread_sleep.h>

#include "SDL3/SDL.h"
#include "SDL3/SDL_vulkan.h"

// #include "vk_types.h"
// #include "vk_initializers.h"

constexpr bool useValidationLayers = true;

BaleineEngine* LOADED_ENGINE = nullptr;

BaleineEngine& BaleineEngine::get() {
    return *LOADED_ENGINE;
}

void BaleineEngine::init() {
    assert(LOADED_ENGINE == nullptr);
    LOADED_ENGINE = this;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

    window = SDL_CreateWindow("Baleine Engine", window_extent.width,
                              window_extent.height, window_flags);

    is_initialized = true;
}

void BaleineEngine::run() {
    SDL_Event event;
    bool should_quit = false;

    while (!should_quit) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_EVENT_QUIT) {
                should_quit = true;
            }

            if (event.window.type == SDL_EVENT_WINDOW_MINIMIZED) {
                is_stop_rendering = true;
            }
            if (event.window.type == SDL_EVENT_WINDOW_RESTORED) {
                is_stop_rendering = false;
            }
        }

        if (is_stop_rendering) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        draw();
    }
}

void BaleineEngine::draw() {
}

void BaleineEngine::cleanup() {
    if (is_initialized) {
        SDL_DestroyWindow(window);
    }

    LOADED_ENGINE = nullptr;
}
