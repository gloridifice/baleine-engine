#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <SDL3/SDL.h>

#include "Image.h"
#include "SurfaceState.h"
#include "../baleine_type/memory.h"
#include "../baleine_type/primitive.h"

// 重载位运算符的宏
#define ENABLE_BITMASK_OPERATORS(EnumType) \
inline EnumType operator|(EnumType a, EnumType b) { \
using T = std::underlying_type_t<EnumType>; \
return static_cast<EnumType>(static_cast<T>(a) | static_cast<T>(b)); \
} \
inline EnumType operator&(EnumType a, EnumType b) { \
using T = std::underlying_type_t<EnumType>; \
return static_cast<EnumType>(static_cast<T>(a) & static_cast<T>(b)); \
} \
inline EnumType operator^(EnumType a, EnumType b) { \
using T = std::underlying_type_t<EnumType>; \
return static_cast<EnumType>(static_cast<T>(a) ^ static_cast<T>(b)); \
} \
inline EnumType operator~(EnumType a) { \
using T = std::underlying_type_t<EnumType>; \
return static_cast<EnumType>(~static_cast<T>(a)); \
} \
inline EnumType& operator|=(EnumType& a, EnumType b) { \
a = a | b; \
return a; \
} \
inline EnumType& operator&=(EnumType& a, EnumType b) { \
a = a & b; \
return a; \
} \
inline EnumType& operator^=(EnumType& a, EnumType b) { \
a = a ^ b; \
return a; \
}

namespace balkan {
    enum class ImageUsage : uint32_t {
        TRANSFER_SRC = VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        TRANSFER_DST = VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        SAMPLED = VK_IMAGE_USAGE_SAMPLED_BIT,
        STORAGE = VK_IMAGE_USAGE_STORAGE_BIT,
        COLOR_ATTACHMENT = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        DEPTH_STENCIL_ATTACHMENT = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        TRANSIENT_ATTACHMENT = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
        INPUT_ATTACHMENT = VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
        HOST_TRANSFER = VK_IMAGE_USAGE_HOST_TRANSFER_BIT,
        VIDEO_DECODE_DST_KHR = VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR,
        VIDEO_DECODE_SRC_KHR = VK_IMAGE_USAGE_VIDEO_DECODE_SRC_BIT_KHR,
        VIDEO_DECODE_DPB_KHR = VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR,
        FRAGMENT_DENSITY_MAP_EXT = VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT,
        FRAGMENT_SHADING_RATE_ATTACHMENT_KHR = VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR,
        VIDEO_ENCODE_DST_KHR = VK_IMAGE_USAGE_VIDEO_ENCODE_DST_BIT_KHR,
        VIDEO_ENCODE_SRC_KHR = VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR,
        VIDEO_ENCODE_DPB_KHR = VK_IMAGE_USAGE_VIDEO_ENCODE_DPB_BIT_KHR,
        ATTACHMENT_FEEDBACK_LOOP_EXT = VK_IMAGE_USAGE_ATTACHMENT_FEEDBACK_LOOP_BIT_EXT,
        INVOCATION_MASK_HUAWEI = VK_IMAGE_USAGE_INVOCATION_MASK_BIT_HUAWEI,
        SAMPLE_WEIGHT_QCOM = VK_IMAGE_USAGE_SAMPLE_WEIGHT_BIT_QCOM,
        SAMPLE_BLOCK_MATCH_QCOM = VK_IMAGE_USAGE_SAMPLE_BLOCK_MATCH_BIT_QCOM,
        TILE_MEMORY_QCOM = VK_IMAGE_USAGE_TILE_MEMORY_QCOM,
        VIDEO_ENCODE_QUANTIZATION_DELTA_MAP_KHR = VK_IMAGE_USAGE_VIDEO_ENCODE_QUANTIZATION_DELTA_MAP_BIT_KHR,
        VIDEO_ENCODE_EMPHASIS_MAP_KHR = VK_IMAGE_USAGE_VIDEO_ENCODE_EMPHASIS_MAP_BIT_KHR,

        // 兼容别名
        SHADING_RATE_IMAGE_NV = VK_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV,
        HOST_TRANSFER_EXT = VK_IMAGE_USAGE_HOST_TRANSFER_BIT_EXT,
        FLAG_BITS_MAX_ENUM = VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM
    };

    ENABLE_BITMASK_OPERATORS(ImageUsage);

    struct ImageCreateInfo {
        VkFormat format;
        ImageUsage usages;
        VkExtent3D extent;
    };

    class RenderState {
    public:
        VkInstance instance;
        VkDevice device;
        VkPhysicalDevice physical_device;
        VmaAllocator allocator;
        VkQueue queue;

        u32 queue_family;

        RenderState();

        //TODO Region
        Shared<Image> create_image(ImageCreateInfo&& info) const;
        Shared<SurfaceState> create_surface_by_sdl_window(SDL_Window* window) const;
        ~RenderState();
    };
}
