//
// Created by geoco on 22.04.2021.
//

#ifndef BASILEVS_BULLETS_H
#define BASILEVS_BULLETS_H

#include <basilevs.h>

struct World;

struct BulletBase;
struct NormalBullet;
template<typename T>
concept is_a_bullet = std::is_base_of_v<BulletBase, T> && requires { T::update_function; };
template<is_a_bullet T>
struct BulletPool;

struct BulletBase {
};

struct NormalBullet : BulletBase {
    using BulletUpdateFunction = std::function<bool(float time, NormalBullet &bullet, World &world)>;

public:
    bool active{false};
    Vector2 position{0.0f, 0.0f};
    Vector2 direction{0.0f, 0.0f};
    float speed{5.0f};
    float timer{0.0f};
    float rotation{0.0f};
    BulletUpdateFunction update_function{[&](float time, BulletBase &bullet, World &world) -> bool { return false; }};
    NormalBullet() : active{false}, position{0.0f, 0.0f}, direction{0.0, 0.0f}, timer{0.0f}, update_function{[&](float time, BulletBase &bullet, World &world) -> bool { return false; }} {};
    explicit NormalBullet(Vector2 position, Vector2 direction, BulletUpdateFunction update_function)
            : position{position}, direction{direction}, update_function{std::move(update_function)} {};
};
#endif//BASILEVS_BULLETS_H
