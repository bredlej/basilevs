add_executable(test_blueprints test_blueprints.cpp)
target_link_libraries(test_blueprints gtest basilevs-lib)

add_test(Blueprints-Test test_blueprints)