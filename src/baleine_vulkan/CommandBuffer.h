#pragma once
#include <vulkan/vulkan.h>

#include "Image.h"

namespace balkan {
    class CommandBuffer {
    private:
        VkCommandBuffer cmd;
        friend class SurfaceState;

    public:
        explicit CommandBuffer(VkCommandBuffer cmd);
        ~CommandBuffer();
        void reset();
        void begin();
        void end();

        void transition_image(Image& image, ImageLayout targe_layout);
        void copy_image_to_image(const Image& src, const Image& dst, VkExtent2D src_extent, VkExtent2D dst_extent);
        void clear_color_image(const Image& image, ImageLayout layout, VkClearColorValue clear_color);
    };

} // balkan
