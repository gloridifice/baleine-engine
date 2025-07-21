#include "baleine_vulkan/CommandBuffer.h"

#include "baleine_vulkan/CommandPool.h"
#include "baleine_vulkan/Device.h"
#include "baleine_vulkan/vk_shared/vk_initializers.h"
#include "baleine_vulkan/vk_shared/vk_utils.h"

namespace balkan {
void CommandBuffer::begin() const {
    auto command_buffer_begin_info = vkinit::command_buffer_begin_info(
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    );
    vkBeginCommandBuffer(vk_command_buffer, &command_buffer_begin_info);
}

void CommandBuffer::end() const {
    vkEndCommandBuffer(vk_command_buffer);
}

void CommandBuffer::transition_image(
    Image& image,
    ImageLayout targe_layout
) const {
    vkutils::transition_image(
        vk_command_buffer,
        image.image,
        static_cast<VkImageLayout>(image.layout),
        static_cast<VkImageLayout>(targe_layout)
    );
    image.layout = targe_layout;
}

CommandBuffer::CommandBuffer(
    VkCommandBuffer cmd,
    Shared<CommandPool>&& command_pool
) :
    swapchain_index(0),
    vk_command_buffer(cmd),
    command_pool(command_pool) {}

CommandBuffer::~CommandBuffer() {
    vkFreeCommandBuffers(
        command_pool->get_device().vk_device,
        command_pool->vk_command_pool,
        1,
        &vk_command_buffer
    );
}

void CommandBuffer::reset() const {
    vkResetCommandBuffer(vk_command_buffer, 0);
}

void CommandBuffer::copy_image_to_image(
    Image& src,
    Image& dst,
    VkExtent3D src_extent,
    VkExtent3D dst_extent,
    bool keep_src_layout,
    bool keep_dst_layout
) const {
    const auto src_layout = src.layout;
    const auto dst_layout = dst.layout;
    if (src_layout != ImageLayout::TransferSrcOptimal)
        transition_image(src, ImageLayout::TransferSrcOptimal);
    if (dst_layout != ImageLayout::TransferDstOptimal)
        transition_image(dst, ImageLayout::TransferDstOptimal);
    vkutils::copy_image_to_image(
        vk_command_buffer,
        src.image,
        dst.image,
        src_extent,
        dst_extent
    );

    if (keep_src_layout)
        transition_image(src, src_layout);
    if (keep_dst_layout)
        transition_image(dst, dst_layout);
}

void CommandBuffer::clear_color_image(
    const Image& image,
    const VkClearColorValue clear_color
) const {
    const auto clear_range =
        vkinit::image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);

    vkCmdClearColorImage(
        vk_command_buffer,
        image.image,
        static_cast<VkImageLayout>(image.layout),
        &clear_color,
        1,
        &clear_range
    );
}
} // namespace balkan
