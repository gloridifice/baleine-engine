#include "Image.h"

#include <stdexcept>

namespace balkan {
    Image::Image(VkImage image, ImageFormat format, VkExtent3D extent, VkDevice device,
                 VmaAllocation allocation,
                 VmaAllocator allocator,
                 ImageLayout layout):
        image(image), format(format), extent(extent), device(device), layout(layout), allocation(allocation),
        allocator(allocator) {
    }

    Image::~Image() {
        if (image && device) {
            if (allocation && allocator)
                vmaDestroyImage(allocator, image, allocation);
            else
                vkDestroyImage(device, image, nullptr);
        }
        else {
            if (!device)
                throw std::logic_error("Device is invalid when destroy image!");
            throw std::logic_error("Image is invalid when destroy image!");
        }
    }

    ImageView::ImageView(VkImageView view, Image& image) : view(view), image(image) {
    }

    ImageView::~ImageView() {
        if (image.device && view)
            vkDestroyImageView(image.device, view, nullptr);
        else {
            if (!image.device)
                throw std::logic_error("Device is invalid when destroy image view!");
            throw std::logic_error("View is invalid when destroy image view!");
        }
    }
}
