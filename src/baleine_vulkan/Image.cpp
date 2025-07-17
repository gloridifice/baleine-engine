
#include "Image.h"

namespace balkan {
    Image::~Image() {
        vmaDestroyImage(allocator, image, allocation);
    }

    ImageView::ImageView(VkImageView view, Image& image) : view(view), image(image){
    }

    ImageView::~ImageView() {
        vkDestroyImageView(image.device, view, nullptr);
    }
}

