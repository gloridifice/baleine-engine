find_package(Vulkan REQUIRED)

add_library(BaleineRender
        src/baleine_render/Renderer.cpp
        include/baleine_render/baleine_render.h
)

target_include_directories(BaleineRender PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(BaleineRender PUBLIC
        # Workspace
        BaleineType
        BaleineVulkan
        # Third party
        Vulkan::Vulkan
        glm::glm
        SDL3::SDL3
        GPUOpen::VulkanMemoryAllocator
        fmt::fmt
        vk-bootstrap::vk-bootstrap
)
