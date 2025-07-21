add_library(BaleineType INTERFACE)

target_include_directories(BaleineType INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(BaleineType PROPERTIES LINKER_LANGUAGE CXX)