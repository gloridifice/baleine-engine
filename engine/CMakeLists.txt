cmake_minimum_required(VERSION 3.31)

set(CMAKE_CXX_STANDARD 20)

project(baleine_engine)

add_subdirectory(third_party)
add_subdirectory(baleine_type)
add_subdirectory(baleine_vulkan)
add_subdirectory(baleine_render)
add_subdirectory(src)
