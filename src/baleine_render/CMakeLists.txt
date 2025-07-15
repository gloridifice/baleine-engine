
add_library(BaleineRender
        RenderState.h
        RenderState.cpp
        vk_shared/vk_initializers.h
        vk_shared/vk_initializers.cpp
        vk_shared/vk_utils.h
        vk_shared/vk_utils.cpp
)

target_include_directories(BaleineRender PUBLIC ${Vulkan_INCLUDE_DIRS})

target_link_libraries(BaleineRender PUBLIC
        Vulkan::Vulkan
        glm::glm
        SDL3::SDL3
        GPUOpen::VulkanMemoryAllocator
        fmt::fmt
        vk-bootstrap::vk-bootstrap
)
