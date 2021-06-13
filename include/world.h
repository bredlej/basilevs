//
// Created by geoco on 22.04.2021.
//

#ifndef BASILEVS_WORLD_H
#define BASILEVS_WORLD_H
//#include "background.h"
//#include "basilevs_player.h"
#include "blueprints.h"
//#include "enemy.h"
//#include "spawn.h"
//#include <basilevs.h>
//#include <bulletpool.h>
//#include <bullets.h>

#include <utility>
#include <list>
#include <memory>
//using namespace basilevs;

/* Forward declarations */
struct BlueprintBase;
struct MemoryBase;

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
    using PlayerType = Blueprint<components::Sprite, components::Movement>;
    explicit TWorld() = default;
public:
    Background background = nullptr;
    std::shared_ptr<PlayerType> player = nullptr;
    std::shared_ptr<MemoryBase> enemies = nullptr;
    BlueprintsInPool<components::Movement> enemy_bullets{1000};
};
#endif//BASILEVS_WORLD_H
