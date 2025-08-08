#pragma once

#include "Device.h"
#include "baleine_type/memory.h"
#include "vulkan/vulkan.h"

namespace balkan {
class Device;

class Fence {
private:
    Shared<Device> device;

public:
    VkFence vk_fence;
    explicit Fence(VkFence vk_fence, Shared<Device>&& device);

    void wait(f64 timeout_sec) const;

    ~Fence();
};

class Semaphore {
  private:
    Shared<Device> device;

  public:
    VkSemaphore vk_semaphore;
    explicit Semaphore(VkSemaphore vk_semaphore, Shared<Device>&& device);
    ~Semaphore();
};

} // namespace balkan