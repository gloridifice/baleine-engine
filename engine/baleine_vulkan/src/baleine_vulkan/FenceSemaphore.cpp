#include "baleine_vulkan/FenceSemaphore.h"

namespace balkan {

Fence::Fence(VkFence vk_fence, Shared<Device>&& device) :
    vk_fence(vk_fence),
    device(device) {}

void Fence::wait(const f64 timeout_sec) const {
    vkWaitForFences(
        device->vk_device,
        1,
        &vk_fence,
        true,
        static_cast<u64>(timeout_sec * 1000000000.0)
    );
}

Fence::~Fence() {
    vkDestroyFence(device->vk_device, vk_fence, nullptr);
}

Semaphore::Semaphore(VkSemaphore vk_semaphore, Shared<Device>&& device) :
    vk_semaphore(vk_semaphore),
    device(device) {}

Semaphore::~Semaphore() {
    vkDestroySemaphore(device->vk_device, vk_semaphore, nullptr);
}
} // namespace balkan