find_package(Vulkan REQUIRED)

add_library(BaleineRender INTERFACE
        include/baleine_render/baleine_render.h)

target_include_directories(BaleineRender INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(BaleineRender PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(BaleineRender INTERFACE
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
        VulkanHpp
)
