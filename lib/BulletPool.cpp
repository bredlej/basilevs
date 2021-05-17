//
// Created by geoco on 22.04.2021.
//
#include <bulletpool.h>
/*
template<>
void BulletPool<NormalBullet>::add(NormalBullet &&bullet) {
    if (first_available_index < pool.size()) {
        auto &bullet_at_index = pool[first_available_index];
        bullet_at_index.active = true;
        bullet_at_index.timer = 0.0f;
        bullet_at_index.position = bullet.position;
        bullet_at_index.direction = bullet.direction;
        bullet_at_index.update_function = bullet.update_function;
        first_available_index += 1;
    }
}
*/