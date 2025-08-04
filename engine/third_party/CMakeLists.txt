if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/auto/CMakeLists.txt")
    add_subdirectory(auto)
else()
    message(STATUS "Skipping auto (CMakeLists.txt not found)")
endif()

add_library(STBImage stb_image/stb_image.h)
add_subdirectory(bitmask-1.1.1)
set_target_properties(STBImage PROPERTIES LINKER_LANGUAGE CXX)

find_package(Vulkan REQUIRED)

SET(GLM_TEST_ENABLE OFF CACHE BOOL "GLM Build unit tests")

