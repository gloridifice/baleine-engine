# Project Libs

# Exec
add_executable(BaleineEngine main.cpp
        BaleineEngine.cpp
        BaleineEngine.h
        )

target_link_libraries(BaleineEngine PRIVATE
        BaleineRender
        BaleineType
        BaleineVulkan
)