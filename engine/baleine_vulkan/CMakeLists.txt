
find_package(Vulkan REQUIRED)

add_library(BaleineVulkan
        src/baleine_vulkan/Image.cpp
        src/baleine_vulkan/RenderState.cpp
        src/baleine_vulkan/SurfaceState.cpp
        src/baleine_vulkan/CommandBuffer.cpp
        src/baleine_vulkan/vk_shared/vk_initializers.cpp
        src/baleine_vulkan/vk_shared/vk_utils.cpp
        src/baleine_vulkan/Instance.cpp
        src/baleine_vulkan/CommandPool.cpp
        src/baleine_vulkan/Device.cpp
)

target_link_libraries(BaleineVulkan PUBLIC
        BaleineType
        GPUOpen::VulkanMemoryAllocator
        Vulkan::Vulkan
        vk-bootstrap::vk-bootstrap
        SDL3::SDL3
        fmt::fmt
)

target_include_directories(BaleineVulkan PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)