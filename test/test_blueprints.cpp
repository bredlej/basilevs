//
// Created by geoco on 08.05.2021.
//
#include <gtest/gtest.h>
#include <basilevs-lib.h>
#include <components.h>

class BlueprintTest : public testing::Test {
protected:
    BlueprintTest() = default;
};

class BlueprintsInMemoryTest : public testing::Test {
protected:
    BlueprintsInMemoryTest() = default;
};

TEST_F(BlueprintTest, CanRunComponentFunctionWithOneComponent)
{
    static constexpr auto kMovementFunction = [](const double time, World &, components::MovementComponent &movement_component) -> void {
      movement_component.position.x += time;
      movement_component.position.y += time;
    };
    auto w = World{};
    auto blueprint = EntityBlueprint<components::MovementComponent>{1, kMovementFunction};
    auto movement_component = get<components::MovementComponent>(blueprint);
    EXPECT_TRUE(movement_component.position.x == 0);
    EXPECT_TRUE(movement_component.position.y == 0);
    blueprint.component_function(1, w, movement_component);
    EXPECT_TRUE(movement_component.position.x == 1);
    EXPECT_TRUE(movement_component.position.y == 1);
    blueprint.component_function(1, w, movement_component);
    EXPECT_TRUE(movement_component.position.x == 2);
    EXPECT_TRUE(movement_component.position.y == 2);
}

TEST_F(BlueprintTest, CanRunComponentFunctionWithManyComponents)
{
    static constexpr auto kMovementFunction = [](const double time, World &, components::MovementComponent &movement_component, components::ActiveComponent &activeComponent) -> void {
      movement_component.position.x += time;
      movement_component.position.y += time;
      activeComponent.is_active = true;
    };
    auto world = World{};
    auto blueprint = EntityBlueprint<components::MovementComponent, components::ActiveComponent>(1, kMovementFunction);
    auto movement_component = get<components::MovementComponent>(blueprint);
    auto active_component = get<components::ActiveComponent>(blueprint);
    EXPECT_TRUE(movement_component.position.x == 0);
    EXPECT_TRUE(movement_component.position.y == 0);
    EXPECT_FALSE(active_component.is_active);
    blueprint.component_function(1, world, movement_component,active_component);
    EXPECT_TRUE(movement_component.position.x == 1);
    EXPECT_TRUE(movement_component.position.y == 1);
    EXPECT_TRUE(active_component.is_active);
    blueprint.component_function(1, world, movement_component, active_component);
    EXPECT_TRUE(movement_component.position.x == 2);
    EXPECT_TRUE(movement_component.position.y == 2);
}

TEST_F(BlueprintsInMemoryTest, CreatesMemoryFromOneBlueprint)
{
    static constexpr auto kMovementFunction = [](const double time, World &, components::MovementComponent &movement_component, components::ActiveComponent &activeComponent) -> void {
      movement_component.position.x += time;
      movement_component.position.y += time;
      activeComponent.is_active = true;
    };
    auto world = World{};
    auto blueprint = EntityBlueprint<components::MovementComponent, components::ActiveComponent>(1, kMovementFunction);
    auto memory = BlueprintsInMemory(blueprint);
    EXPECT_EQ(1, get<components::MovementComponent>(memory).size());
    EXPECT_EQ(1, get<components::ActiveComponent>(memory).size());
}

TEST_F(BlueprintsInMemoryTest, CreatesMemoryFromManyBlueprints)
{
    static constexpr auto kMovementFunction = [](const double time, World &, components::MovementComponent &movement_component, components::ActiveComponent &activeComponent) -> void {
      movement_component.position.x += time;
      movement_component.position.y += time;
      activeComponent.is_active = true;
    };
    auto world = World{};
    auto blueprint1 = EntityBlueprint<components::MovementComponent, components::ActiveComponent>(1, kMovementFunction);
    auto &blueprint1_movement_component = get<components::MovementComponent>(blueprint1);
    blueprint1_movement_component.position.x = 10;
    auto blueprint2 = EntityBlueprint<components::MovementComponent, components::ActiveComponent>(2, kMovementFunction);
    auto blueprint3 = EntityBlueprint<components::MovementComponent, components::ActiveComponent>(3, kMovementFunction);
    auto memory = BlueprintsInMemory(blueprint1, blueprint2, blueprint3);
    EXPECT_EQ(3, get<components::MovementComponent>(memory).size());
    EXPECT_EQ(3, get<components::ActiveComponent>(memory).size());
    auto first_movement_component_in_memory = get<components::MovementComponent>(memory)[0];
    EXPECT_EQ(10, first_movement_component_in_memory.position.x);
}


int main(int ac, char* av[])
{
    testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}