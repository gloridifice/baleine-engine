add_library(BaleineType INTERFACE)

target_include_directories(BaleineType INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(BaleineType INTERFACE
        absl::synchronization 
)

set_target_properties(BaleineType PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(test)