#pragma once

#include <vulkan/vulkan.h>

#include "Image.h"

namespace balkan {
class CommandPool;
class Device;

class CommandBuffer: EnableSharedFromThis<CommandBuffer> {
  private:
    friend class SurfaceState;

    u32 swapchain_index;

    Shared<CommandPool> command_pool;

  public:
    VkCommandBuffer vk_command_buffer;

    explicit CommandBuffer(
        VkCommandBuffer cmd,
        Shared<CommandPool>&& command_pool
    );

    ~CommandBuffer();
    void reset() const;
    void begin() const;
    void end() const;

    void transition_image(Image& image, ImageLayout targe_layout) const;
    void copy_image_to_image(
        Image& src,
        Image& dst,
        VkExtent3D src_extent,
        VkExtent3D dst_extent,
        bool keep_src_layout = false,
        bool keep_dst_layout = false
    ) const;
    void
    clear_color_image(const Image& image, VkClearColorValue clear_color) const;
};

} // namespace balkan
