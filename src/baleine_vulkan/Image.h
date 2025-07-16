#pragma once

#include <vulkan/vulkan.h>

#include "vk_mem_alloc.h"

namespace balkan {
    class ImageView;

    class Image {
    public:
        VkImage image;
        VkFormat format;
        VkExtent3D extent_3d;
        VmaAllocation allocation;

        VkDevice device;
        VmaAllocator allocator;

        ~Image();

        ImageView create_view();
    };

    class ImageView {
    public:
        VkImageView view;

        Image* image;

        ~ImageView();
    };
}

