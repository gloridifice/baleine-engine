
#include "Image.h"

namespace balkan {
    Image::~Image() {
        vmaDestroyImage(allocator, image, allocation);
    }

    ImageView::~ImageView() {
        vkDestroyImageView(image->device, view, nullptr);
    }
}

