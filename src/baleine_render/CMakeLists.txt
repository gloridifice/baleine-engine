
add_library(BaleineRender
        Renderer.h
        Renderer.cpp
        vk_shared/vk_initializers.h
        vk_shared/vk_initializers.cpp
        vk_shared/vk_utils.h
        vk_shared/vk_utils.cpp
        AllocatedImage.cpp
        AllocatedImage.h
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
