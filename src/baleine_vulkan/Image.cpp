#include "Image.h"

namespace balkan {
    Image::Image(VkImage image, Format format, VkExtent3D extent, VkDevice device,
                 VmaAllocation allocation,
                 VmaAllocator allocator,
                 ImageLayout layout):
        image(image), format(format), extent(extent), device(device), layout(layout), allocation(allocation),
        allocator(allocator) {
    }

    Image::~Image() {
        vmaDestroyImage(allocator, image, allocation);
    }

    ImageView::ImageView(VkImageView view, Image& image) : view(view), image(image) {
    }

    ImageView::~ImageView() {
        vkDestroyImageView(image.device, view, nullptr);
    }
}
