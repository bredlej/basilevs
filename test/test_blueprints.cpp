//
// Created by geoco on 08.05.2021.
//
#include <basilevs-lib.h>
#include <components.h>
#include <gtest/gtest.h>

class BlueprintTest : public testing::Test {
protected:
    BlueprintTest() = default;
};

class BlueprintsInMemoryTest : public testing::Test {
protected:
    BlueprintsInMemoryTest() = default;
};

class ComponentTest : public testing::Test {
protected:
    ComponentTest() = default;
};

class StateComponentTest : public ComponentTest {
protected:
    StateComponentTest() = default;
};

TEST_F(BlueprintTest, CanRunComponentFunctionWithOneComponent) {
    static constexpr auto kUpdateFunction = [](const double time, TWorld &, components::Movement &movement_component) -> void {
        movement_component.position.x += time;
        movement_component.position.y += time;
    };
    auto w = TWorld{};
    auto blueprint = EntityBlueprint<components::Movement>{1, kUpdateFunction};
    auto movement_component = get<components::Movement>(blueprint);
    EXPECT_TRUE(movement_component.position.x == 0);
    EXPECT_TRUE(movement_component.position.y == 0);
    blueprint.component_function(1, w, movement_component);
    EXPECT_TRUE(movement_component.position.x == 1);
    EXPECT_TRUE(movement_component.position.y == 1);
    blueprint.component_function(1, w, movement_component);
    EXPECT_TRUE(movement_component.position.x == 2);
    EXPECT_TRUE(movement_component.position.y == 2);
}

TEST_F(BlueprintTest, CanRunComponentFunctionWithManyComponents) {
    static constexpr auto kUpdateFunction = [](const double time, TWorld &, components::Movement &movement_component, components::Activation &activeComponent) -> void {
        movement_component.position.x += time;
        movement_component.position.y += time;
        activeComponent.is_active = true;
    };
    auto world = TWorld{};
    auto blueprint = EntityBlueprint<components::Movement, components::Activation>(1, kUpdateFunction);
    auto movement_component = get<components::Movement>(blueprint);
    auto active_component = get<components::Activation>(blueprint);
    EXPECT_TRUE(movement_component.position.x == 0);
    EXPECT_TRUE(movement_component.position.y == 0);
    EXPECT_FALSE(active_component.is_active);
    blueprint.component_function(1, world, movement_component, active_component);
    EXPECT_TRUE(movement_component.position.x == 1);
    EXPECT_TRUE(movement_component.position.y == 1);
    EXPECT_TRUE(active_component.is_active);
    blueprint.component_function(1, world, movement_component, active_component);
    EXPECT_TRUE(movement_component.position.x == 2);
    EXPECT_TRUE(movement_component.position.y == 2);
}

TEST_F(BlueprintsInMemoryTest, CreatesMemoryFromOneBlueprint) {
    static constexpr auto kUpdateFunction = [](const double time, TWorld &, components::Movement &movement_component, components::Activation &activeComponent) -> void {
        movement_component.position.x += time;
        movement_component.position.y += time;
        activeComponent.is_active = true;
    };
    auto world = TWorld{};
    auto blueprint = EntityBlueprint<components::Movement, components::Activation>(1, kUpdateFunction);
    auto memory = BlueprintsInMemory(blueprint);
    EXPECT_EQ(1, get<components::Movement>(memory).size());
    EXPECT_EQ(1, get<components::Activation>(memory).size());
}

TEST_F(BlueprintsInMemoryTest, CreatesMemoryFromManyBlueprints) {
    static constexpr auto kUpdateFunction = [](const double time, TWorld &, components::Movement &movement_component, components::Activation &activeComponent) -> void {
        movement_component.position.x += time;
        movement_component.position.y += time;
        activeComponent.is_active = true;
    };
    auto world = TWorld{};
    auto blueprint1 = EntityBlueprint<components::Movement, components::Activation>(1, kUpdateFunction);
    auto &blueprint1_movement_component = get<components::Movement>(blueprint1);
    blueprint1_movement_component.position.x = 10;
    auto blueprint2 = EntityBlueprint<components::Movement, components::Activation>(2, kUpdateFunction);
    auto blueprint3 = EntityBlueprint<components::Movement, components::Activation>(3, kUpdateFunction);
    auto memory = BlueprintsInMemory(blueprint1, blueprint2, blueprint3);
    EXPECT_EQ(3, get<components::Movement>(memory).size());
    EXPECT_EQ(3, get<components::Activation>(memory).size());
    auto first_movement_component_in_memory = get<components::Movement>(memory)[0];
    EXPECT_EQ(10, first_movement_component_in_memory.position.x);
}

TEST_F(BlueprintsInMemoryTest, UpdatesAllBlueprintsInMemory) {
    static constexpr auto kUpdateFunction = [](const double time, TWorld &, components::Movement &movement_component, components::Activation &activeComponent) -> void {
        movement_component.position.x += time;
        movement_component.position.y += time;
        activeComponent.is_active = true;
    };
    auto world = TWorld{};
    auto blueprint1 = EntityBlueprint<components::Movement, components::Activation>(1, kUpdateFunction);
    auto blueprint2 = EntityBlueprint<components::Movement, components::Activation>(2, kUpdateFunction);
    auto blueprint3 = EntityBlueprint<components::Movement, components::Activation>(3, kUpdateFunction);
    auto memory = BlueprintsInMemory(blueprint1, blueprint2, blueprint3);
    for (int i = 0; i < 3; i++) {
        auto &movement_component = get<components::Movement>(memory)[i];
        auto &active_component = get<components::Activation>(memory)[i];
        EXPECT_EQ(0, movement_component.position.x);
        EXPECT_EQ(0, movement_component.position.y);
        EXPECT_FALSE(active_component.is_active);
    }
    memory.update(1, world);
    for (int i = 0; i < 3; i++) {
        auto &movement_component = get<components::Movement>(memory)[i];
        auto &active_component = get<components::Activation>(memory)[i];
        EXPECT_EQ(1, movement_component.position.x);
        EXPECT_EQ(1, movement_component.position.y);
        EXPECT_TRUE(active_component.is_active);
    }
}

TEST_F(StateComponentTest, DoesStateComponentTransitionBetweenStates) {
    using namespace sml;
    using namespace state;
    struct InitEvent {};
    struct RunEvent {};
    struct StopEvent {};

    struct StatefulObject {};

    struct StateMachineDeclaration {
        auto operator()() const {
            return make_transition_table(
                    *"entry"_s + event<InitEvent> = "init"_s,
                    "init"_s + event<RunEvent> = "running"_s,
                    "running"_s + event<StopEvent> = X);
        }
    };

    static constexpr auto kUpdateFunction = [](const double time, TWorld &, components::StateMachine<StateMachineDeclaration, StatefulObject> &stateComponent) -> void {
        if (time == 1) {
            stateComponent.state_machine.process_event(InitEvent{});
        } else if (time == 2) {
            stateComponent.state_machine.process_event(RunEvent());
        } else {
            stateComponent.state_machine.process_event(StopEvent());
        }
    };

    auto world = TWorld();
    auto blueprint = EntityBlueprint<components::StateMachine<StateMachineDeclaration, StatefulObject>>(0, kUpdateFunction);
    auto &state_component = get<components::StateMachine<StateMachineDeclaration, StatefulObject>>(blueprint);

    EXPECT_TRUE(state_component.state_machine.is("entry"_s));
    blueprint.component_function(1, world, state_component);

    EXPECT_TRUE(state_component.state_machine.is("init"_s));
    blueprint.component_function(2, world, state_component);

    EXPECT_TRUE(state_component.state_machine.is("running"_s));
    blueprint.component_function(1, world, state_component);

    EXPECT_TRUE(state_component.state_machine.is("running"_s));
    blueprint.component_function(3, world, state_component);

    EXPECT_TRUE(state_component.state_machine.is(X));
}

TEST_F(StateComponentTest, DoComponentsChangeStateAfterGivenTime)
{
    std::cout << "------ Do ComponentsChangeStateAfterGivenTime" << std::endl;
    using namespace sml;
    using namespace state;
    struct InitEvent {};
    struct RunEvent {};
    struct StopEvent {};

    std::cout << "> Declaring struct StatefulObjectT" << std::endl;
    struct StatefulObjectT {
        int i = 0;
    };

    std::cout << "> Declaring initialize" << std::endl;
    static constexpr auto initialize = [&](const auto &event, StatefulObjectT &stateful_object) {
      stateful_object.i = 10;
      std::cout << "OBJECT IN LAMBDA: " << &stateful_object << std::endl;
    };

    std::cout << "> Declaring struct StateMachineDeclaration" << std::endl;
    struct StateMachineDeclaration {
        auto operator()() const {
            return make_transition_table(
                    *"entry"_s + event<InitEvent> / initialize = "init"_s,
                    "init"_s + event<RunEvent> = "running"_s,
                    "running"_s + event<StopEvent> = X);
        }
    };

    std::cout << "> Declaring kUpdateFunction" << std::endl;
    static constexpr auto kUpdateFunction = [](const double time, TWorld &, components::StateMachine<StateMachineDeclaration, StatefulObjectT> &stateComponent) -> void {
      if (time == 1) {
          stateComponent.state_machine.process_event(InitEvent{});
      } else if (time == 2) {
          stateComponent.state_machine.process_event(RunEvent());
      } else {
          stateComponent.state_machine.process_event(StopEvent());
      }
    };

    auto world = TWorld();
    std::cout << "> Creating blueprint" << std::endl;
    auto blueprint = EntityBlueprint<components::StateMachine<StateMachineDeclaration, StatefulObjectT>>(0, kUpdateFunction);
    std::cout << "> Accessing component" << std::endl;
    auto &state_component = get<components::StateMachine<StateMachineDeclaration, StatefulObjectT>>(blueprint);
    std::cout << "OBJECT IN COMPONENT: " << &state_component.obj << std::endl;
    EXPECT_TRUE(state_component.state_machine.is("entry"_s));

    std::cout << "> Accessing component #2" << std::endl;
    auto &state_component2 = get<components::StateMachine<StateMachineDeclaration, StatefulObjectT>>(blueprint);

    blueprint.component_function(1, world, state_component2);
    std::cout << "OBJECT IN COMPONENT #2: " << &state_component2.obj << std::endl;
    EXPECT_TRUE(state_component.state_machine.is("init"_s));
    blueprint.component_function(2, world, state_component);

    EXPECT_TRUE(state_component.state_machine.is("running"_s));
    blueprint.component_function(1, world, state_component);

    EXPECT_TRUE(state_component.state_machine.is("running"_s));
    blueprint.component_function(3, world, state_component);

    EXPECT_TRUE(state_component.state_machine.is(X));
}

int main(int ac, char *av[]) {
    testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}