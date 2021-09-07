//
// Created by geoco on 07.09.2021.
//

#ifndef BASILEVS_STATE_HANDLING_H
#define BASILEVS_STATE_HANDLING_H
namespace state_handling
{
    using namespace boost::sml;

    struct StatefulObject {
    };

    namespace events
    {
        struct DestroyEvent {
        };
        struct DamageEvent {
        };
        struct RecoverEvent {
        };
        struct ArrivalEvent {
        };
        struct StartEvent {
        };
        struct KillEvent {
        };
    }// namespace events

    namespace declarations
    {
        constexpr auto kShootState = "shoot"_s;
        constexpr auto kAliveState = "alive"_s;
        constexpr auto kTakingDamageState = "taking_damage"_s;
        constexpr auto kDeadState = "dead"_s;
        constexpr auto kInitState = "init"_s;
        constexpr auto kArrivalState = "arrival"_s;
    }// namespace declarations

    namespace transitions
    {
        using namespace declarations;
        using namespace events;
        struct BulletPossibleStates {
            auto operator()() const
            {
                return make_transition_table(
                        *kShootState + event<DestroyEvent> = X);
            }
        };

        struct PlayerPossibleStates {
            auto operator()() const
            {
                return make_transition_table(
                        *kAliveState + event<DamageEvent> = kTakingDamageState,
                        kTakingDamageState + event<RecoverEvent> = kAliveState,
                        kTakingDamageState + event<KillEvent> = kDeadState,
                        kDeadState + event<DestroyEvent> = X,
                        kAliveState + event<DestroyEvent> = X);
            }
        };

        struct EnemyPossibleStates {
            auto operator()() const
            {
                return make_transition_table(
                        *kInitState + event<ArrivalEvent> = kArrivalState,
                        kArrivalState + event<StartEvent> = kAliveState,
                        kArrivalState + event<DamageEvent> = kTakingDamageState,
                        kAliveState + event<DamageEvent> = kTakingDamageState,
                        kTakingDamageState + event<RecoverEvent> = kAliveState,
                        kTakingDamageState + event<KillEvent> = kDeadState,
                        kDeadState + event<DestroyEvent> = X,
                        kAliveState + event<DestroyEvent> = X);
            }
        };
    }// namespace transitions
}// namespace state_handling
#endif//BASILEVS_STATE_HANDLING_H
