#pragma once

#include "vulkan/vulkan.h"

namespace vkutils {
    void transition_image(VkCommandBuffer cmd, VkImage image,
        VkImageLayout current_layout, VkImageLayout target_layout);
}
