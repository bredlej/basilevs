add_library(basilevs-lib basilevs-lib.cpp level-loader.cpp)
target_link_libraries(basilevs-lib raylib raylib-cpp nlohmann_json::nlohmann_json)
target_include_directories(basilevs-lib
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/extern
)
