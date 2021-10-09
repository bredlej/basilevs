cmake_minimum_required(VERSION 3.16)
project(basilevs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_subdirectory(extern)
add_subdirectory(lib)
include(FetchContent)
include(CTest)
enable_testing()
add_subdirectory(test)
add_executable(basilevs main.cpp)
set(BUILD_RAYLIB_CPP_EXAMPLES ON)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.10.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


#FetchContent_Declare(
#        gperftools
#        GIT_REPOSITORY https://github.com/gperftools/gperftools
#        GIT_TAG        gperftools-2.9.1
#)
#FetchContent_MakeAvailable(gperftools)

if(WIN32)
    # Windows: all libraries are built-in
    add_compile_options(/DUNICODE) # Unicode app
    if(NOT ${CUSTOM_FLAGS} MATCHES ".*PL_IMPL_STACKTRACE=0.*")
        message("Palanteer feature 'stacktrace' enabled for testprogram")
        add_definitions(-DPL_IMPL_STACKTRACE=1)
    endif()
else(WIN32)
    if(NOT CUSTOM_FLAGS MATCHES ".*PL_IMPL_STACKTRACE=0.*")
        # Linux: if libunwind and libdw are present, the stacktrace feature is activated
        find_package(LibUnwind)
        find_package(LibDw)
        if (LIBUNWIND_FOUND AND LIBDW_FOUND)
            add_definitions(-DPL_IMPL_STACKTRACE=1)
            message("Palanteer feature 'stacktrace' enabled for testprogram")
            set(STACKTRACE_LIBS ${LIBUNWIND_LIBRARY} ${LIBDW_LIBRARY} )
        endif()
    endif()
endif(WIN32)
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
