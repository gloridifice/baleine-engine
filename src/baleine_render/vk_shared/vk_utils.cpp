//
// Created by koiro on 2025/7/14.
//

#include "vk_utils.h"

#include "vk_initializers.h"

void vkutils::transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout current_layout,
                               VkImageLayout target_layout) {
    VkImageMemoryBarrier2 image_memory_barrier2{
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2
    };
    image_memory_barrier2.pNext = nullptr;
    // ALL_COMMANDS 意味着 GPU 会停止所有其它命令直到它到达了 Barrier
    image_memory_barrier2.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    image_memory_barrier2.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
    image_memory_barrier2.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    image_memory_barrier2.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

    image_memory_barrier2.oldLayout = current_layout;
    image_memory_barrier2.newLayout = target_layout;

    VkImageAspectFlags aspect_mask = (target_layout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
                                         ? VK_IMAGE_ASPECT_DEPTH_BIT
                                         : VK_IMAGE_ASPECT_COLOR_BIT;
    image_memory_barrier2.subresourceRange = vkinit::image_subresource_range(aspect_mask);
    image_memory_barrier2.image = image;

    VkDependencyInfo dependency_info{
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO
    };
    dependency_info.pNext = nullptr;
    dependency_info.imageMemoryBarrierCount = 1;
    dependency_info.pImageMemoryBarriers = &image_memory_barrier2;

    vkCmdPipelineBarrier2(cmd, &dependency_info);
}
