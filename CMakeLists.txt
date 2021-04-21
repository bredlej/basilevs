cmake_minimum_required(VERSION 3.17)
project(basilevs)

set(CMAKE_CXX_STANDARD 20)
add_subdirectory(extern)
add_subdirectory(lib)
add_executable(basilevs main.cpp)

target_link_libraries(basilevs basilevs-lib)
add_custom_command(
        TARGET basilevs POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_CURRENT_BINARY_DIR}/assets)