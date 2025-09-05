
add_executable(TestBaleineType test.cpp)

target_link_libraries(TestBaleineType PRIVATE doctest::doctest BaleineType)