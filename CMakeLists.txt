cmake_minimum_required(VERSION 3.16)
project(basilevs)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_subdirectory(extern)
add_subdirectory(SRC)
include(FetchContent)
include(CTest)
include(cmake/CPM.cmake)
include(cmake/fetch-json.cmake)
include(cmake/fetch-googletest.cmake)
include(cmake/fetch-entt.cmake)

enable_testing()
add_subdirectory(test)
add_executable(basilevs main.cpp)
set(BUILD_RAYLIB_CPP_EXAMPLES ON)
set(RAYLIB_VERSION 5.5)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
    include(FetchContent)
    FetchContent_Declare(
            raylib
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_MakeAvailable(raylib)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    endif()
endif()

set(IMGUI_SOURCES
        extern/imgui/imgui.cpp
        extern/imgui/imgui_demo.cpp
        extern/imgui/imgui_draw.cpp
        extern/imgui/imgui_widgets.cpp
        extern/imgui/imgui_tables.cpp
        extern/imgui/imgui.h
        extern/imgui/imgui_internal.h
        extern/imgui/misc/cpp/imgui_stdlib.cpp
        extern/imgui/misc/cpp/imgui_stdlib.h)
add_library(imgui ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC extern/imgui)

add_library(rlimgui extern/rlimgui/rlImGui.cpp extern/rlimgui/rlImGui.h)
target_link_libraries(rlimgui PUBLIC imgui raylib)
target_include_directories(rlimgui PUBLIC raylib/src/external/glfw/include)


add_definitions(-DUSE_PL=0)
# Multithreading is used (both client and server applications)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(basilevs basilevs-lib ${STACKTRACE_LIBS} Threads::Threads)
add_custom_command(
        TARGET basilevs POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_CURRENT_BINARY_DIR}/assets)
