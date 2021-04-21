add_library(basilevs-lib basilevs-lib.cpp)
target_include_directories(basilevs-lib
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/extern
)
