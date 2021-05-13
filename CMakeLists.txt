cmake_minimum_required(VERSION 3.17)
project(basilevs)

set(CMAKE_CXX_STANDARD 20)
add_subdirectory(extern)
add_subdirectory(lib)
add_subdirectory(test)
add_executable(basilevs main.cpp)

include(FetchContent)
include(CTest)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.10.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

#FetchContent_Declare(
#        gperftools
#        GIT_REPOSITORY https://github.com/gperftools/gperftools
#        GIT_TAG        gperftools-2.9.1
#)
#FetchContent_MakeAvailable(gperftools)

target_link_libraries(basilevs basilevs-lib)
add_custom_command(
        TARGET basilevs POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_CURRENT_BINARY_DIR}/assets)