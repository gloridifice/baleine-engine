# Project Libs
#add_library(baleine_render)
#add_library(baleine_vulkan)
set(SDL3_DIR "C:/Libs/sdl/SDL3-3.2.16/cmake" CACHE PATH "SDL3 CMake Config")

find_package(SDL3 REQUIRED)
find_package(Vulkan REQUIRED)

include_directories(${SDL3_INCLUDE_DIRS})

add_subdirectory(baleine_render)
add_subdirectory(baleine_type)

# Exec
add_executable(BaleineEngine main.cpp
        BaleineEngine.cpp
        BaleineEngine.h
        )

add_subdirectory(third_party/glfw-3.4)
SET(GLM_TEST_ENABLE OFF CACHE BOOL "GLM Build unit tests")
add_subdirectory(third_party/glm-1.0.1 EXCLUDE_FROM_ALL)
add_subdirectory(third_party/VulkanMemoryAllocator-3.3.0)
add_subdirectory(third_party/fmt-11.2.0)
add_subdirectory(third_party/vk-bootstrap)

target_include_directories(BaleineEngine PRIVATE third_party/stb_image)
target_link_libraries(BaleineEngine PRIVATE
        BaleineRender
)