# Project Libs

# Exec
add_executable(BaleineEngine main.cpp
        RenderContext.cpp
        RenderContext.h
        BaleineEngine.cpp
        BaleineEngine.h
)

target_link_libraries(BaleineEngine PRIVATE
        BaleineRender
        BaleineType
        BaleineVulkan
        VulkanHpp
)
