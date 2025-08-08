#pragma once

#include <vulkan/vulkan.h>
#include <fmt/format.h>

#define VK_CHECK(x)                                                     \
do {                                                                \
VkResult err = x;                                               \
if (err) {                                                      \
fmt::print("Detected Vulkan error: {}", err); \
abort();                                                    \
}                                                               \
} while (0)

template <>
struct fmt::formatter<VkResult> : fmt::formatter<std::string_view> {
    auto format(VkResult result, format_context& ctx) const {
        const char* str = "Unknown VkResult";
        switch (result) {
        case VK_SUCCESS: str = "VK_SUCCESS"; break;
        case VK_NOT_READY: str = "VK_NOT_READY"; break;
        case VK_TIMEOUT: str = "VK_TIMEOUT"; break;
        case VK_EVENT_SET: str = "VK_EVENT_SET"; break;
        case VK_EVENT_RESET: str = "VK_EVENT_RESET"; break;
        case VK_INCOMPLETE: str = "VK_INCOMPLETE"; break;
        case VK_ERROR_OUT_OF_HOST_MEMORY: str = "VK_ERROR_OUT_OF_HOST_MEMORY"; break;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY: str = "VK_ERROR_OUT_OF_DEVICE_MEMORY"; break;
            // 添加更多 VkResult 枚举值...
        default: break;
        }
        return fmt::formatter<std::string_view>::format(str, ctx);
    }
};
