//
// Created by geoco on 10.10.2021.
//

#ifndef BASILEVS_INITIALIZATION_H
#define BASILEVS_INITIALIZATION_H
#include <vector>
#include <assets.h>
#include <world.h>
#include <components.h>
#include <behaviours/enemy.h>
#include <behaviours/player.h>
#include <behaviours/background.h>



namespace initialization
{
    using namespace components;

    enum class AnimationDirection
            {
        Vertical,
        Horizontal
            };

    TWorld::PlayerType create_player(const std::vector<Texture2D> &textures);

    TWorld::EnemyType create_enemy(const double seconds_until_spawns, const Vector2 &position, const behaviours::enemy::EnemyDefinition &enemy_definition);

    TWorld::EnemyType create_enemy_with_sprite(const double seconds_until_spawns, const std::vector<Texture2D> &textures, const Vector2 &position, const behaviours::enemy::EnemyDefinition &enemy_definition);

    TWorld::BackgroundType create_background(const std::vector<Texture2D> &textures);
}// namespace initialization
#endif//BASILEVS_INITIALIZATION_H
