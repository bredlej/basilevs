add_executable(test_blueprints test_blueprints.cpp)
target_link_libraries(test_blueprints gtest basilevs-lib)

add_executable(test_json_loader test_json_loader.cpp)
target_link_libraries(test_json_loader gtest basilevs-lib)

add_test(Blueprints-Test test_blueprints)
add_test(Blueprints-Test test_json_loader)