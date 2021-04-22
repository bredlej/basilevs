add_library(basilevs-lib basilevs-lib.cpp World.cpp Bullets.cpp BulletPool.cpp Spawn.cpp Sprite.cpp Enemy.cpp BasilevsPlayer.cpp Background.cpp Emitter.cpp)
target_link_libraries(basilevs-lib raylib raylib-cpp)
target_include_directories(basilevs-lib
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/extern
)
