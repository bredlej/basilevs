//
// Created by geoco on 22.04.2021.
//

#ifndef BASILEVS_ENEMY_H
#define BASILEVS_ENEMY_H


#include "Emitter.h"
#include "sprite.h"
#include <functional>
struct Enemy {
    using EnemyBehaviorFunction = std::function<void(Enemy &, double)>;
public:
    Enemy() = default;
    Enemy(Sprite animation, EnemyBehaviorFunction behavior, Vector2 position, Emitter emitter) : animation{animation}, behavior{std::move(behavior)}, position{position}, emitter{std::move(emitter)} {};
    Sprite animation;
    Emitter emitter;
    Vector2 emitter_offset{11, 13};
    EnemyBehaviorFunction behavior;
    Vector2 position{0.0f, 0.0f};
};
#endif//BASILEVS_ENEMY_H
