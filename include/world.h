//
// Created by geoco on 22.04.2021.
//

#ifndef BASILEVS_WORLD_H
#define BASILEVS_WORLD_H
#include "Emitter.h"
#include "background.h"
#include "basilevs_player.h"
#include "enemy.h"
#include "spawn.h"
#include <basilevs.h>
#include <bulletpool.h>
#include <bullets.h>

#include <utility>
#include <list>
#include <memory>
using namespace basilevs;
/*
struct World;

struct World {
    World() = default;
    explicit World(BasilevsPlayer player, const Background &&background, const Rectangle bounds, const BulletPool<NormalBullet> &&enemy_bullets, const BulletPool<NormalBullet> &&player_bullets) : player{std::move(player)}, background{background}, bounds{bounds}, enemy_bullets{enemy_bullets}, player_bullets{player_bullets} {};

public:
    void update(std::chrono::duration<double> duration);
    void render(const Sprite &bullet_sprite);

    Background background {Background{}};
    BasilevsPlayer player {BasilevsPlayer{}};
    raylib::Rectangle bounds;
    BulletPool<NormalBullet> enemy_bullets;
    BulletPool<NormalBullet> player_bullets;
    std::list<Spawn> enemy_spawns;
    std::vector<Enemy> enemies_on_screen;
    double timer{0.0f};
    void spawn_enemies_();
    void update_bullets_(const std::chrono::duration<double> &elapsed);
    void update_emitters_(const std::chrono::duration<double> &elapsed);
    void render_bullets_(const Sprite &bullet_sprite);
};
*/
/*concept is_a_blueprint;
concept is_a_memory;*/
struct BlueprintBase;
struct MemoryBase;
struct TWorld {
    static constexpr auto install(auto entity) { return std::make_unique<decltype(entity)>(entity); }
    explicit TWorld() = default;
    explicit TWorld(is_a_blueprint auto player_blueprint, is_a_memory auto enemies_memory)
        : player(install(player_blueprint)),
          enemies(install(enemies_memory)) {};
public:
    std::unique_ptr<BlueprintBase> player = nullptr;
    std::unique_ptr<MemoryBase> enemies = nullptr;
};
#endif//BASILEVS_WORLD_H
