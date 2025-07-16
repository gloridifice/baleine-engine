//
// Created by koiro on 2025/7/15.
//

#pragma once

#include <vulkan/vulkan.h>

#include "vk_mem_alloc.h"

struct AllocatedImage {
    VkImage image;
    VkImageView view;
    VmaAllocation allocation;
    VkExtent3D extent;
    VkFormat format;
};
