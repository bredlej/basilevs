//
// Created by geoco on 08.05.2021.
//
#include <game-definition.h>
#include <components.h>
#include <../include/pool.hpp>
#include <gtest/gtest.h>

class PoolTest : public testing::Test {
protected:
    PoolTest() = default;
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

TEST_F(PoolTest, RunsComponentFunctionWithOneComponent)
{
    Pool<int> pool(10);
    pool.add(1);
    pool.add(2);
    pool.add(3);

    for (const auto a : pool.get_occupied()) {
        std::printf("%d\n", a);
    }
}

TEST_F(StateComponentTest, DoesStateComponentTransitionBetweenStates)
{
    using namespace sml;
    using namespace state_handling;
    struct InitEvent {};
    struct RunEvent {};
    struct StopEvent {};

    struct StatefulObject {};

    struct StateMachineDeclaration {
        auto operator()() const
        {
            return make_transition_table(
                    *"entry"_s + event<InitEvent> = "init"_s,
                    "init"_s + event<RunEvent> = "running"_s,
                    "running"_s + event<StopEvent> = X);
        }
    };

    static constexpr auto kUpdateFunction = [](const double time, TWorld &, components::StateMachine<StateMachineDeclaration, StatefulObject> &stateComponent) -> void { if (time == 1) { stateComponent.state_machine.process_event(InitEvent{}); } else if (time == 2) { stateComponent.state_machine.process_event(RunEvent()); } else { stateComponent.state_machine.process_event(StopEvent()); } };

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

/*
TEST_F(StateComponentTest, DoStatesChangeAfterSpecificTime)
{
    using namespace sml;
    using namespace state_handling;
    struct InitEvent {};
    struct RunEvent {};
    struct StopEvent {};

    struct StatefulObjectT {
        int i = 0;
    };

    static constexpr auto initialize = [&](const auto &event, StatefulObjectT &stateful_object) { stateful_object.i = 10; };

    struct StateMachineDeclaration {
        auto operator()() const
        {
            return make_transition_table(
                    *"entry"_s + event<InitEvent> / initialize = "init"_s,
                    "init"_s + event<RunEvent> = "running"_s,
                    "running"_s + event<StopEvent> = X);
        }
    };

    static constexpr auto kUpdateFunction = [](const double time, TWorld &, components::TimeCounter &timeCounter, components::Activation activation, components::StateMachine<StateMachineDeclaration, StatefulObjectT> &stateComponent) -> void
    {
        timeCounter.elapsed_seconds += time;
        if (stateComponent.state_machine.is("entry"_s) && timeCounter.elapsed_seconds >= activation.activate_after_seconds) {
            stateComponent.state_machine.process_event(InitEvent{});
            stateComponent.state_machine.process_event(RunEvent());
        }
    };

    auto world = TWorld();
    auto bp_activate_after_1s = Blueprint<components::TimeCounter, components::Activation, components::StateMachine<StateMachineDeclaration, StatefulObjectT>>(kUpdateFunction);
    auto bp_activate_after_2s = Blueprint<components::TimeCounter, components::Activation, components::StateMachine<StateMachineDeclaration, StatefulObjectT>>(kUpdateFunction);

    auto &bp1_activation = get<components::Activation>(bp_activate_after_1s);
    bp1_activation.activate_after_seconds = 1.0;

    auto &bp2_activation = get<components::Activation>(bp_activate_after_2s);
    bp2_activation.activate_after_seconds = 2.0;

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
*/
int main(int ac, char *av[])
{
    testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}