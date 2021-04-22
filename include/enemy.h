//
// Created by geoco on 22.04.2021.
//

#ifndef BASILEVS_ENEMY_H
#define BASILEVS_ENEMY_H


#include "Emitter.h"
#include "sprite.h"
#include <functional>
struct Enemy {
public:
    Enemy() = default;
    Enemy(Sprite animation, std::function<void(Enemy &, double)> behavior, Vector2 position, Emitter emitter) : animation{animation}, behavior{std::move(behavior)}, position{position}, emitter{std::move(emitter)} {};
    Sprite animation;
    Emitter emitter;
    Vector2 emitter_offset{11, 13};
    std::function<void(Enemy &, double)> behavior;
    Vector2 position;
};
#endif//BASILEVS_ENEMY_H
