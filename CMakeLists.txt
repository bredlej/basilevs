cmake_minimum_required(VERSION 3.17)
project(basilevs)

set(CMAKE_CXX_STANDARD 20)
add_subdirectory(extern)
add_executable(basilevs main.cpp)
target_link_libraries(basilevs raylib-cpp raylib)