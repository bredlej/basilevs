//
// Created by geoco on 22.04.2021.
//

#ifndef BASILEVS_WORLD_H
#define BASILEVS_WORLD_H
#include "blueprints.h"
#include "state_handling.h"
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

    using BackgroundType = Blueprint<
            components::Sprite,
            components::Movement>;

    explicit TWorld() = default;

public:
    std::shared_ptr<PlayerType> player = nullptr;
    std::shared_ptr<EnemyListType> enemies = nullptr;
    std::shared_ptr<BackgroundType> background = nullptr;
    BulletPool player_bullets{config::kPlayerBulletPoolSize};
    BulletPool enemy_bullets{config::kEnemyBulletPoolSize};
    input::UserInput<input::PlayerInput> player_input;
    std::vector<assets::SoundId> sounds_queue{config::kSoundQueueSize};
    const raylib::Rectangle frame_bounds{config::kFrameBoundLeft, config::kFrameBoundUp, 260, 260};
};
#endif//BASILEVS_WORLD_H
