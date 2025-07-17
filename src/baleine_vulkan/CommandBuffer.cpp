
#include "CommandBuffer.h"

#include "../baleine_render/vk_shared/vk_initializers.h"
#include "../baleine_render/vk_shared/vk_utils.h"

namespace balkan {
    void CommandBuffer::begin() {
        auto command_buffer_begin_info = vkinit::command_buffer_begin_info(
            VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
            );
        vkBeginCommandBuffer(cmd, &command_buffer_begin_info);
    }

    void CommandBuffer::end() {
        vkEndCommandBuffer(cmd);
    }

    void CommandBuffer::transition_image(Image& image, ImageLayout targe_layout) {
        vkutils::transition_image(cmd, image.image, static_cast<VkImageLayout>(image.layout), static_cast<VkImageLayout>(targe_layout));
        image.layout = targe_layout;
    }

    CommandBuffer::CommandBuffer(VkCommandBuffer cmd): cmd(cmd) {
    }

    CommandBuffer::~CommandBuffer() {
    }

    void CommandBuffer::reset() {
        vkResetCommandBuffer(cmd, 0);
    }

    void CommandBuffer::copy_image_to_image(const Image& src, const Image& dst, VkExtent2D src_extent, VkExtent2D dst_extent) {
        vkutils::copy_image_to_image(cmd, src.image, dst.image, src_extent, dst_extent);
    }

    void CommandBuffer::clear_color_image(const Image& image, ImageLayout layout, const VkClearColorValue clear_color) {
        const auto clear_range = vkinit::image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);

        vkCmdClearColorImage(cmd, image.image,
                             static_cast<VkImageLayout>(layout), &clear_color, 1, &clear_range);
    }
} // balkan