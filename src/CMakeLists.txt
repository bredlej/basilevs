set(SOURCE_FILES
        game-definition.cpp
        level-loader.cpp
)
add_library(basilevs-lib ${SOURCE_FILES})
target_link_libraries(basilevs-lib raylib EnTT nlohmann_json::nlohmann_json imgui rlimgui)
target_include_directories(basilevs-lib
        PUBLIC
        .
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/extern
)
