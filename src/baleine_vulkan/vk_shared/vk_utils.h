#pragma once

#include "vulkan/vulkan.h"

namespace vkutils {
    void transition_image(VkCommandBuffer cmd, VkImage image,
        VkImageLayout current_layout, VkImageLayout target_layout);

    void copy_image_to_image(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent3D srcSize, VkExtent3D dstSize);
}
