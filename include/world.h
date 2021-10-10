//
// Created by geoco on 22.04.2021.
//

#ifndef BASILEVS_WORLD_H
#define BASILEVS_WORLD_H
#include <blueprints.h>
#include <state_handling.h>
#include <input.h>
#include <list>
#include <memory>
#include <utility>
/* Forward declarations */
struct BlueprintBase;
struct MemoryBase;

/*
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

#define ENEMY_COMPONENTS      \
    components::Sprite,       \
    components::Movement,     \
    components::MovementPath, \
    components::Activation,   \
    components::TimeCounter,  \
    components::Emission,     \
    components::Collision,    \
    components::Health,       \
    components::StateMachine<state_handling::transitions::EnemyPossibleStates, state_handling::StatefulObject>

#define PLAYER_COMPONENTS  \
    components::Sprite,    \
    components::Movement,  \
    components::Emission,  \
    components::Collision, \
    components::Health,    \
    components::StateMachine<state_handling::transitions::PlayerPossibleStates, state_handling::StatefulObject>

#define BULLET_COMPONENTS                                                                                        \
    components::Sprite,                                                                                          \
    components::Movement,                                                                                        \
    components::StateMachine<state_handling::transitions::BulletPossibleStates, state_handling::StatefulObject>, \
    components::TimeCounter,                                                                                     \
    components::Collision,                                                                                       \
    components::Damage

#define BACKGROUND_COMPONENTS \
    components::Sprite, \
    components::Movement

    using PlayerType = Blueprint<PLAYER_COMPONENTS>;
    using EnemyType = Blueprint<ENEMY_COMPONENTS>;
    using EnemiesInMemory = BlueprintsInMemory<ENEMY_COMPONENTS>;
    using BulletPool = BlueprintsInPool<BULLET_COMPONENTS>;
    using BackgroundType = Blueprint<BACKGROUND_COMPONENTS>;

    using PlayerStateComponent = components::StateMachine<state_handling::transitions::PlayerPossibleStates, state_handling::StatefulObject>;
    using EnemyStateComponent = components::StateMachine<state_handling::transitions::EnemyPossibleStates, state_handling::StatefulObject>;
    using BulletStateComponent = components::StateMachine<state_handling::transitions::BulletPossibleStates, state_handling::StatefulObject>;

    explicit TWorld() = default;

public:
    std::shared_ptr<PlayerType> player = nullptr;
    std::shared_ptr<EnemiesInMemory> enemies = nullptr;
    std::shared_ptr<BackgroundType> background = nullptr;
    BulletPool player_bullets{config::kPlayerBulletPoolSize};
    BulletPool enemy_bullets{config::kEnemyBulletPoolSize};
    input::UserInput<input::PlayerInput> player_input;
    std::vector<assets::SoundId> sounds_queue{config::kSoundQueueSize};
    const raylib::Rectangle frame_bounds{config::kFrameBoundLeft, config::kFrameBoundUp, 260, 260};
};
#endif//BASILEVS_WORLD_H
