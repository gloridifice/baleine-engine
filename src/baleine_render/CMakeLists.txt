
find_package(Vulkan REQUIRED)

add_library(BaleineRender
        Renderer.h
        Renderer.cpp
)

target_include_directories(BaleineRender PUBLIC ${Vulkan_INCLUDE_DIRS})

target_link_libraries(BaleineRender PUBLIC
        # Workspace
        BaleineType
        # Third party
        Vulkan::Vulkan
        glm::glm
        SDL3::SDL3
        GPUOpen::VulkanMemoryAllocator
        fmt::fmt
        vk-bootstrap::vk-bootstrap
)
