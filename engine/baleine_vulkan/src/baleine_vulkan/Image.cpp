#include "baleine_vulkan/Image.h"

#include <stdexcept>
#include <utility>

#include "baleine_vulkan/Device.h"
#include "fmt/args.h"

namespace balkan {
Image::Image(
    VkImage image,
    ImageFormat format,
    VkExtent3D extent,
    Shared<Device>&& device,
    VmaAllocation allocation,
    VmaAllocator allocator,
    ImageLayout layout
) :
    image(image),
    format(format),
    extent(extent),
    device(device),
    layout(layout),
    allocation(allocation),
    allocator(allocator) {}

Image::~Image() {
    if (image != VK_NULL_HANDLE) {
        if (allocation && allocator)
            vmaDestroyImage(allocator, image, allocation);
        else
            vkDestroyImage(device->vk_device, image, nullptr);
    } else {
        throw std::logic_error("Image is invalid when destroy image!");
    }
}

ImageView::ImageView(VkImageView view, Shared<Image>&& image) :
    view(view),
    image(image) {}

ImageView::~ImageView() {
    if (view != VK_NULL_HANDLE)
        vkDestroyImageView(image->device->vk_device, view, nullptr);
    else {
        if (image->device->vk_device == VK_NULL_HANDLE)
            throw std::logic_error(
                "Device is invalid when destroy image view!"
            );
        throw std::logic_error("View is invalid when destroy image view!");
    }
}
} // namespace balkan
