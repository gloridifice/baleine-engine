
find_package(Vulkan REQUIRED)

add_library(BaleineVulkan
        Image.h
        Image.cpp
        RenderState.h
        RenderState.cpp
        SurfaceState.h
        SurfaceState.cpp
        CommandBuffer.h
        CommandBuffer.cpp
        macros/check.h
        macros/bitmask.h
        vk_shared/vk_initializers.h
        vk_shared/vk_initializers.cpp
        vk_shared/vk_utils.h
        vk_shared/vk_utils.cpp
)

target_include_directories(BaleineVulkan PUBLIC ${Vulkan_INCLUDE_DIRS})

target_link_libraries(BaleineVulkan PUBLIC
        BaleineType
        GPUOpen::VulkanMemoryAllocator
        Vulkan::Vulkan
        vk-bootstrap::vk-bootstrap
        SDL3::SDL3
        fmt::fmt
)