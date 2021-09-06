//
// Created by geoco on 22.04.2021.
//

#ifndef BASILEVS_WORLD_H
#define BASILEVS_WORLD_H
#include "blueprints.h"
#include <input.h>
#include <list>
#include <memory>
#include <utility>
/* Forward declarations */
struct BlueprintBase;
struct MemoryBase;

namespace state_handling {
    using namespace boost::sml;

    struct StatefulObject {};

    namespace events {
        struct DestroyEvent {};
        struct DamageEvent {};
        struct RecoverEvent {};
        struct ArrivalEvent {};
        struct StartEvent {};
        struct KillEvent {};
    }

    namespace declarations {
        constexpr auto kShootState = "shoot"_s;
        constexpr auto kAliveState = "alive"_s;
        constexpr auto kTakingDamageState = "taking_damage"_s;
        constexpr auto kDeadState = "dead"_s;
        constexpr auto kInitState = "init"_s;
        constexpr auto kArrivalState = "arrival"_s;
    }

    namespace transitions {
        using namespace declarations;
        using namespace events;
        struct BulletPossibleStates {
            auto operator()() const {
                return make_transition_table(
                        *kShootState + event<DestroyEvent> = X);
            }
        };

        struct PlayerPossibleStates {
            auto operator()() const {
                return make_transition_table(
                        *kAliveState + event<DamageEvent> = kTakingDamageState,
                        kTakingDamageState + event<RecoverEvent> = kAliveState,
                        kTakingDamageState + event<KillEvent> = kDeadState,
                        kDeadState + event<DestroyEvent> = X,
                        kAliveState + event<DestroyEvent> = X);
            }
        };

        struct EnemyPossibleStates {
            auto operator()() const {
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
    }

}// namespace state
/**
 * Describes a structure representing the game environment.

 * Holds references to various entities used in the game such as:
 * - the player entity
 * - enemy definitions
 * - bullets
 *
 * Instances of TWorld are referenced by all update functions of Blueprint/BlueprintInMemory/BlueprintInPool objects, which allows specific entities to access their environment.
 * This can be used for implementation of cases like:
 * - Player and enemies when shooting add bullets into the world
 * - Bullets can check for collision with other entities
 *
 */
struct TWorld {
    using Background = std::shared_ptr<Blueprint<components::Sprite, components::Movement>>;

    using PlayerStateComponent = components::StateMachine<state_handling::transitions::PlayerPossibleStates, state_handling::StatefulObject>;
    /*
     * Describes the components a player object consists of
     */
    using PlayerType = Blueprint<
            components::Sprite,
            components::Movement,
            components::Emission,
            components::Collision,
            components::Health,
            PlayerStateComponent>;

    using EnemyStateComponent = components::StateMachine<state_handling::transitions::EnemyPossibleStates, state_handling::StatefulObject>;
    /*
     * Describes the components which all enemies consist of
     */
    using EnemyListType = BlueprintsInMemory<
            components::Sprite,
            components::Movement,
            components::Activation,
            components::TimeCounter,
            components::Emission,
            components::Collision,
            components::Health,
            EnemyStateComponent>;

    using BulletStateComponent = components::StateMachine<state_handling::transitions::BulletPossibleStates, state_handling::StatefulObject>;
    /*
     * Describes the components which all bullets consist of
     */
    using BulletPool = BlueprintsInPool<
            components::Sprite,
            components::Movement,
            BulletStateComponent,
            components::TimeCounter,
            components::Collision,
            components::Damage>;

    explicit TWorld() = default;

public:
    Background background = nullptr;
    BulletPool player_bullets{config::kPlayerBulletPoolSize};
    BulletPool enemy_bullets{config::kEnemyBulletPoolSize};
    std::shared_ptr<PlayerType> player = nullptr;
    std::shared_ptr<EnemyListType> enemies = nullptr;
    input::UserInput<input::PlayerInput> player_input;
    std::vector<assets::SoundId> sounds_queue{config::kSoundQueueSize};
    const raylib::Rectangle frame_bounds{config::kFrameBoundLeft, config::kFrameBoundUp, 260, 260};
};
#endif//BASILEVS_WORLD_H
