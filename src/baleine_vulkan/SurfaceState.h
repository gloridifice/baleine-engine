#pragma once

#include <vulkan/vulkan.h>

namespace balkan {
    class SurfaceState {
        VkInstance instance;
        VkSurfaceKHR surface;
        SurfaceState(VkInstance instance, VkSurfaceKHR surface);
        ~SurfaceState();
    };
}
