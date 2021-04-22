add_library(basilevs-lib basilevs-lib.cpp world.cpp bullets.cpp bulletpool.cpp spawn.cpp sprite.cpp enemy.cpp basilevs_player.cpp background.cpp)
target_link_libraries(basilevs-lib raylib raylib-cpp)
target_include_directories(basilevs-lib
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/extern
)
