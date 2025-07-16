
#include "SurfaceState.h"

balkan::SurfaceState::SurfaceState(VkInstance instance, VkSurfaceKHR surface): instance(instance), surface(surface) {
}

balkan::SurfaceState::~SurfaceState() {
    vkDestroySurfaceKHR(instance, surface, nullptr);
}
