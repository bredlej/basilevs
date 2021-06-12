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

class BlueprintsInPoolTest : public testing::Test {
protected:
    BlueprintsInPoolTest() = default;
};

class ComponentTest : public testing::Test {
protected:
    ComponentTest() = default;
};

class StateComponentTest : public ComponentTest {
protected:
    StateComponentTest() = default;
};

TEST_F(BlueprintTest, RunsComponentFunctionWithOneComponent) {
    static constexpr auto kUpdateFunction = [](const double time, TWorld &, components::Movement &movement_component) -> void {
        movement_component.position.x += time;
        movement_component.position.y += time;
    };
    auto w = TWorld{};
    auto blueprint = Blueprint<components::Movement>{kUpdateFunction};
    auto movement_component = get<components::Movement>(blueprint);
    EXPECT_TRUE(movement_component.position.x == 0);
    EXPECT_TRUE(movement_component.position.y == 0);
    blueprint.update_function(1, w, movement_component);
    EXPECT_TRUE(movement_component.position.x == 1);
    EXPECT_TRUE(movement_component.position.y == 1);
    blueprint.update_function(1, w, movement_component);
    EXPECT_TRUE(movement_component.position.x == 2);
    EXPECT_TRUE(movement_component.position.y == 2);
}

TEST_F(BlueprintTest, RunsComponentFunctionWithManyComponents) {
    static constexpr auto kUpdateFunction = [](const double time, TWorld &, components::Movement &movement_component, components::Activation &activeComponent) -> void {
        movement_component.position.x += time;
        movement_component.position.y += time;
        activeComponent.is_active = true;
    };
    auto world = TWorld{};
    auto blueprint = Blueprint<components::Movement, components::Activation>(kUpdateFunction);
    auto movement_component = get<components::Movement>(blueprint);
    auto active_component = get<components::Activation>(blueprint);
    EXPECT_TRUE(movement_component.position.x == 0);
    EXPECT_TRUE(movement_component.position.y == 0);
    EXPECT_FALSE(active_component.is_active);
    blueprint.update_function(1, world, movement_component, active_component);
    EXPECT_TRUE(movement_component.position.x == 1);
    EXPECT_TRUE(movement_component.position.y == 1);
    EXPECT_TRUE(active_component.is_active);
    blueprint.update_function(1, world, movement_component, active_component);
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
    auto blueprint = Blueprint<components::Movement, components::Activation>(kUpdateFunction);
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
    auto blueprint1 = Blueprint<components::Movement, components::Activation>(kUpdateFunction);
    auto &blueprint1_movement_component = get<components::Movement>(blueprint1);
    blueprint1_movement_component.position.x = 10;
    auto blueprint2 = Blueprint<components::Movement, components::Activation>(kUpdateFunction);
    auto blueprint3 = Blueprint<components::Movement, components::Activation>(kUpdateFunction);
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
    auto blueprint1 = Blueprint<components::Movement, components::Activation>(kUpdateFunction);
    auto blueprint2 = Blueprint<components::Movement, components::Activation>(kUpdateFunction);
    auto blueprint3 = Blueprint<components::Movement, components::Activation>(kUpdateFunction);
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

TEST_F(BlueprintsInPoolTest, CreatesPoolWithGivenSize) {
    auto pool = BlueprintsInPool<components::Movement>(2);
    ASSERT_EQ(2, pool.size);
    ASSERT_EQ(2, get<components::Movement>(pool).size());
    ASSERT_EQ(0, pool.first_available_index);
}

TEST_F(BlueprintsInPoolTest, AddsBlueprintToPool) {
    static constexpr auto kUpdateFunction = [](const double time, TWorld &, components::Movement &movement_component, components::Activation &) -> void {
      movement_component.position.x += time;
      movement_component.position.y += time;
    };
    auto w = TWorld{};
    auto blueprint = Blueprint<components::Movement, components::Activation>{kUpdateFunction};
    auto &movement_component = get<components::Movement>(blueprint);
    movement_component.position.x = 10.0f;
    movement_component.position.y = 10.0f;
    auto pool = BlueprintsInPool<components::Movement, components::Activation>(2);
    pool.add(blueprint);
    ASSERT_EQ(1, pool.first_available_index);
    ASSERT_EQ(10.0f, get<components::Movement>(pool)[0].position.x);
    ASSERT_EQ(10.0f, get<components::Movement>(pool)[0].position.y);
    ASSERT_EQ(0.0f, get<components::Movement>(pool)[1].position.x);
    ASSERT_EQ(0.0f, get<components::Movement>(pool)[1].position.y);
}

TEST_F(BlueprintsInPoolTest, AddsManyBlueprintsToPool) {
    static constexpr auto kUpdateFunction = [](const double time, TWorld &, components::Movement &movement_component, components::Activation &) -> void {
      movement_component.position.x += time;
      movement_component.position.y += time;
    };
    auto w = TWorld{};
    auto blueprint = Blueprint<components::Movement, components::Activation>{kUpdateFunction};
    auto &movement_component = get<components::Movement>(blueprint);
    movement_component.position.x = 10.0f;
    movement_component.position.y = 10.0f;
    auto blueprint2 = Blueprint<components::Movement, components::Activation>{kUpdateFunction};
    auto &movement_component2 = get<components::Movement>(blueprint2);
    movement_component2.position.x = 100.0f;
    movement_component2.position.y = 100.0f;
    auto blueprint3 = Blueprint<components::Movement, components::Activation>{kUpdateFunction};
    auto &movement_component3 = get<components::Movement>(blueprint3);
    movement_component3.position.x = 1000.0f;
    movement_component3.position.y = 1000.0f;
    auto pool = BlueprintsInPool<components::Movement, components::Activation>(3);
    pool.add(blueprint);
    pool.add(blueprint2);
    pool.add(blueprint3);
    ASSERT_EQ(3, pool.first_available_index);
    ASSERT_EQ(10.0f, get<components::Movement>(pool)[0].position.x);
    ASSERT_EQ(10.0f, get<components::Movement>(pool)[0].position.y);
    ASSERT_EQ(100.0f, get<components::Movement>(pool)[1].position.x);
    ASSERT_EQ(100.0f, get<components::Movement>(pool)[1].position.y);
    ASSERT_EQ(1000.0f, get<components::Movement>(pool)[2].position.x);
    ASSERT_EQ(1000.0f, get<components::Movement>(pool)[2].position.y);
}

TEST_F(BlueprintsInPoolTest, RemovesFirstBlueprintFromPool) {
    static constexpr auto kUpdateFunction = [](const double time, TWorld &, components::Movement &movement_component, components::Activation &) -> void {
      movement_component.position.x += time;
      movement_component.position.y += time;
    };
    auto w = TWorld{};
    auto blueprint = Blueprint<components::Movement, components::Activation>{kUpdateFunction};
    auto &movement_component = get<components::Movement>(blueprint);
    movement_component.position.x = 10.0f;
    movement_component.position.y = 10.0f;
    auto blueprint2 = Blueprint<components::Movement, components::Activation>{kUpdateFunction};
    auto &movement_component2 = get<components::Movement>(blueprint2);
    movement_component2.position.x = 100.0f;
    movement_component2.position.y = 100.0f;
    auto pool = BlueprintsInPool<components::Movement, components::Activation>(2);
    pool.add(blueprint);
    pool.add(blueprint2);
    pool.remove_at(0);
    ASSERT_EQ(1, pool.first_available_index);
    ASSERT_EQ(100.0f, get<components::Movement>(pool)[0].position.x);
    ASSERT_EQ(100.0f, get<components::Movement>(pool)[0].position.y);
    ASSERT_EQ(100.0f, get<components::Movement>(pool)[1].position.x);
    ASSERT_EQ(100.0f, get<components::Movement>(pool)[1].position.y);
}

TEST_F(BlueprintsInPoolTest, RemovesLastBlueprintFromPool) {
    static constexpr auto kUpdateFunction = [](const double time, TWorld &, components::Movement &movement_component, components::Activation &) -> void {
      movement_component.position.x += time;
      movement_component.position.y += time;
    };
    auto w = TWorld{};
    auto blueprint = Blueprint<components::Movement, components::Activation>{kUpdateFunction};
    auto &movement_component = get<components::Movement>(blueprint);
    movement_component.position.x = 10.0f;
    movement_component.position.y = 10.0f;
    auto blueprint2 = Blueprint<components::Movement, components::Activation>{kUpdateFunction};
    auto &movement_component2 = get<components::Movement>(blueprint2);
    movement_component2.position.x = 100.0f;
    movement_component2.position.y = 100.0f;
    auto pool = BlueprintsInPool<components::Movement, components::Activation>(2);
    pool.add(blueprint);
    pool.add(blueprint2);
    pool.remove_at(1);
    ASSERT_EQ(1, pool.first_available_index);
    ASSERT_EQ(10.0f, get<components::Movement>(pool)[0].position.x);
    ASSERT_EQ(10.0f, get<components::Movement>(pool)[0].position.y);
    ASSERT_EQ(100.0f, get<components::Movement>(pool)[1].position.x);
    ASSERT_EQ(100.0f, get<components::Movement>(pool)[1].position.y);
}

TEST_F(BlueprintsInPoolTest, RemovesMiddleBlueprintFromPool) {
    static constexpr auto kUpdateFunction = [](const double time, TWorld &, components::Movement &movement_component, components::Activation &) -> void {
      movement_component.position.x += time;
      movement_component.position.y += time;
    };
    auto w = TWorld{};
    auto blueprint = Blueprint<components::Movement, components::Activation>{kUpdateFunction};
    auto &movement_component = get<components::Movement>(blueprint);
    movement_component.position.x = 10.0f;
    movement_component.position.y = 10.0f;
    auto blueprint2 = Blueprint<components::Movement, components::Activation>{kUpdateFunction};
    auto &movement_component2 = get<components::Movement>(blueprint2);
    movement_component2.position.x = 100.0f;
    movement_component2.position.y = 100.0f;
    auto blueprint3 = Blueprint<components::Movement, components::Activation>{kUpdateFunction};
    auto &movement_component3 = get<components::Movement>(blueprint3);
    movement_component3.position.x = 1000.0f;
    movement_component3.position.y = 1000.0f;
    auto pool = BlueprintsInPool<components::Movement, components::Activation>(3);
    pool.add(blueprint);
    pool.add(blueprint2);
    pool.add(blueprint3);
    pool.remove_at(1);
    ASSERT_EQ(2, pool.first_available_index);
    ASSERT_EQ(10.0f, get<components::Movement>(pool)[0].position.x);
    ASSERT_EQ(10.0f, get<components::Movement>(pool)[0].position.y);
    ASSERT_EQ(1000.0f, get<components::Movement>(pool)[1].position.x);
    ASSERT_EQ(1000.0f, get<components::Movement>(pool)[1].position.y);
}


TEST_F(BlueprintsInPoolTest, UpdatesOneBlueprintInPool) {
    static constexpr auto kUpdateFunction = [](const double time, TWorld &, components::Movement &movement_component, components::Activation &) -> void {
      movement_component.position.x += time;
      movement_component.position.y += time;
    };
    auto w = TWorld{};
    auto blueprint = Blueprint<components::Movement, components::Activation>{kUpdateFunction};
    auto &movement_component = get<components::Movement>(blueprint);
    movement_component.position.x = 10.0f;
    movement_component.position.y = 10.0f;
    auto pool = BlueprintsInPool<components::Movement, components::Activation>(2);
    pool.add(blueprint);
    ASSERT_EQ(1, pool.first_available_index);
    ASSERT_EQ(10.0f, get<components::Movement>(pool)[0].position.x);
    ASSERT_EQ(10.0f, get<components::Movement>(pool)[0].position.y);
    ASSERT_EQ(0.0f, get<components::Movement>(pool)[1].position.x);
    ASSERT_EQ(0.0f, get<components::Movement>(pool)[1].position.y);
    pool.update(1.0f, w);
    ASSERT_EQ(11.0f, get<components::Movement>(pool)[0].position.x);
    ASSERT_EQ(11.0f, get<components::Movement>(pool)[0].position.y);
    ASSERT_EQ(0.0f, get<components::Movement>(pool)[1].position.x);
    ASSERT_EQ(0.0f, get<components::Movement>(pool)[1].position.y);
}

TEST_F(BlueprintsInPoolTest, UpdatesManyBlueprintsInPool) {
    static constexpr auto kUpdateFunction = [](const double time, TWorld &, components::Movement &movement_component, components::Activation &) -> void {
      movement_component.position.x += time;
      movement_component.position.y += time;
    };
    auto w = TWorld{};
    auto blueprint = Blueprint<components::Movement, components::Activation>{kUpdateFunction};
    auto &movement_component = get<components::Movement>(blueprint);
    movement_component.position.x = 10.0f;
    movement_component.position.y = 10.0f;
    auto blueprint2 = Blueprint<components::Movement, components::Activation>{kUpdateFunction};
    auto &movement_component2 = get<components::Movement>(blueprint2);
    movement_component2.position.x = 100.0f;
    movement_component2.position.y = 100.0f;
    auto pool = BlueprintsInPool<components::Movement, components::Activation>(2);
    pool.add(blueprint);
    pool.add(blueprint2);
    ASSERT_EQ(2, pool.first_available_index);
    ASSERT_EQ(10.0f, get<components::Movement>(pool)[0].position.x);
    ASSERT_EQ(10.0f, get<components::Movement>(pool)[0].position.y);
    ASSERT_EQ(100.0f, get<components::Movement>(pool)[1].position.x);
    ASSERT_EQ(100.0f, get<components::Movement>(pool)[1].position.y);
    pool.update(1.0f, w);
    ASSERT_EQ(11.0f, get<components::Movement>(pool)[0].position.x);
    ASSERT_EQ(11.0f, get<components::Movement>(pool)[0].position.y);
    ASSERT_EQ(101.0f, get<components::Movement>(pool)[1].position.x);
    ASSERT_EQ(101.0f, get<components::Movement>(pool)[1].position.y);
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
    auto blueprint = Blueprint<components::StateMachine<StateMachineDeclaration, StatefulObject>>(kUpdateFunction);
    auto &state_component = get<components::StateMachine<StateMachineDeclaration, StatefulObject>>(blueprint);

    EXPECT_TRUE(state_component.state_machine.is("entry"_s));
    blueprint.update_function(1, world, state_component);

    EXPECT_TRUE(state_component.state_machine.is("init"_s));
    blueprint.update_function(2, world, state_component);

    EXPECT_TRUE(state_component.state_machine.is("running"_s));
    blueprint.update_function(1, world, state_component);

    EXPECT_TRUE(state_component.state_machine.is("running"_s));
    blueprint.update_function(3, world, state_component);

    EXPECT_TRUE(state_component.state_machine.is(X));
}

TEST_F(StateComponentTest, DoStatesChangeAfterSpecificTime) {
    using namespace sml;
    using namespace state;
    struct InitEvent {};
    struct RunEvent {};
    struct StopEvent {};

    struct StatefulObjectT {
        int i = 0;
    };

    static constexpr auto initialize = [&](const auto &event, StatefulObjectT &stateful_object) {
        stateful_object.i = 10;
    };

    struct StateMachineDeclaration {
        auto operator()() const {
            return make_transition_table(
                    *"entry"_s + event<InitEvent> / initialize = "init"_s,
                    "init"_s + event<RunEvent> = "running"_s,
                    "running"_s + event<StopEvent> = X);
        }
    };

    static constexpr auto kUpdateFunction = [](const double time, TWorld &, components::TimeCounter &timeCounter, components::Activation activation, components::StateMachine<StateMachineDeclaration, StatefulObjectT> &stateComponent) -> void {
        timeCounter.elapsed_time += time;
        if (stateComponent.state_machine.is("entry"_s) && timeCounter.elapsed_time >= activation.activate_after_time) {
            stateComponent.state_machine.process_event(InitEvent{});
            stateComponent.state_machine.process_event(RunEvent());
        }
    };

    auto world = TWorld();
    auto bp_activate_after_1s = Blueprint<components::TimeCounter, components::Activation, components::StateMachine<StateMachineDeclaration, StatefulObjectT>>(kUpdateFunction);
    auto bp_activate_after_2s = Blueprint<components::TimeCounter, components::Activation, components::StateMachine<StateMachineDeclaration, StatefulObjectT>>(kUpdateFunction);

    auto &bp1_activation = get<components::Activation>(bp_activate_after_1s);
    bp1_activation.activate_after_time = 1.0;

    auto &bp2_activation = get<components::Activation>(bp_activate_after_2s);
    bp2_activation.activate_after_time = 2.0;

    auto memory = BlueprintsInMemory(bp_activate_after_1s, bp_activate_after_2s);

    auto &states = get<components::StateMachine<StateMachineDeclaration, StatefulObjectT>>(memory);
    auto &activation = get<components::Activation>(memory);
    ASSERT_TRUE(states[0].state_machine.is("entry"_s));
    ASSERT_TRUE(states[1].state_machine.is("entry"_s));

    memory.update(1, world);

    ASSERT_TRUE(states[0].state_machine.is("running"_s));
    ASSERT_TRUE(states[1].state_machine.is("entry"_s));

    memory.update(1, world);
    ASSERT_TRUE(states[0].state_machine.is("running"_s));
    ASSERT_TRUE(states[1].state_machine.is("running"_s));
}

int main(int ac, char *av[]) {
    testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}