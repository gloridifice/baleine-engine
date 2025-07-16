
add_library(BaleineVulkan
        Image.h
        Image.cpp
        RenderState.h
        RenderState.cpp
        SurfaceState.h
        SurfaceState.cpp
)

target_link_libraries(BaleineVulkan
        BaleineType
        GPUOpen::VulkanMemoryAllocator
        Vulkan::Vulkan
        vk-bootstrap::vk-bootstrap
        SDL3::SDL3
)