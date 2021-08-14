//
// Created by geoco on 22.04.2021.
//

#ifndef BASILEVS_WORLD_H
#define BASILEVS_WORLD_H
#include "blueprints.h"
#include <list>
#include <memory>
#include <utility>
#include <input.h>
/* Forward declarations */
struct BlueprintBase;
struct MemoryBase;
namespace state {
    using namespace boost::sml;
    using namespace state;
    struct InitEvent {};
    struct RunEvent {};
    struct StopEvent {};
    struct DestroyEvent {};

    struct StatefulObject {};

    struct StateMachineDeclaration {
        auto operator()() const {
            return make_transition_table(
                    *"entry"_s + event<InitEvent> = "init"_s,
                    "init"_s + event<RunEvent> = "running"_s,
                    "running"_s + event<StopEvent> = X);
        }
    };
    struct BulletState {
        auto operator()() const {
            return make_transition_table(
                    *"shoot"_s + event<DestroyEvent> = X);
        }
    };
}
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
    using PlayerType = Blueprint<components::Sprite, components::Movement, components::Emission>;
    using EnemyListType = BlueprintsInMemory<components::Sprite, components::Movement, components::Activation, components::TimeCounter, components::Emission>;

    explicit TWorld() = default;

public:
    Background background = nullptr;
    std::shared_ptr<PlayerType> player = nullptr;
    std::shared_ptr<EnemyListType> enemies = nullptr;
    BlueprintsInPool<components::Sprite, components::Movement, components::StateMachine<state::BulletState, state::StatefulObject>> enemy_bullets{10};
    BlueprintsInPool<components::Sprite, components::Movement, components::StateMachine<state::BulletState, state::StatefulObject>> player_bullets{10};
    raylib::Rectangle bounds{-32, -32, 180, 180};
    input::UserInput<input::PlayerInput> player_input;
    std::vector<assets::SoundId> sounds_queue{16};
};
#endif//BASILEVS_WORLD_H
