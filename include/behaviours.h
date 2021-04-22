//
// Created by geoco on 12.04.2021.
//

#ifndef BASILEVS_BEHAVIOURS_H
#define BASILEVS_BEHAVIOURS_H

#include "bullets.h"
#include "enemy.h"
namespace basilevs {
    namespace bullet {
        enum class Type {FlyForward, FlyForwardFast, FlySpiral};
        static constexpr auto bullet_fly_forward = [](float time, NormalBullet &bullet, const World &world) -> bool {
            bullet.position = Vector2Add(bullet.position, bullet.direction);

            bullet.timer += time;

            Vector2Normalize(bullet.direction);
            return true;
        };
        static constexpr auto bullet_fly_forward_fast = [](float time, NormalBullet &bullet, const World &world) -> bool {
            //bullet.position = Vector2Add(bullet.position, bullet.direction);
            bullet.position.y += bullet.direction.y * time*200;
            bullet.position.x += bullet.direction.x * time*200;
            bullet.timer += time;

            Vector2Normalize(bullet.direction);
            return true;
        };

        static constexpr auto bullet_fly_spiral = [](const float time, NormalBullet &bullet, const World &world) -> bool {
            bullet.timer += time;
            //if (bullet.timer > 0.1f) {
            bullet.rotation += 0.01f;
            if (bullet.rotation > 360) {
                bullet.rotation = 0;
            }
            bullet.timer = 0.0f;
            //}
            //bullet.position.x += bullet.direction.x ;//

            bullet.position = Vector2Add(bullet.position, bullet.direction);
            //bullet.direction = Vector2Rotate(bullet.direction, bullet.rotation);
            //bullet.direction = Vector2Normalize(bullet.direction);
            return true;
        };
    }// namespace bullet

    namespace enemy {
        constexpr auto behavior_sinusoidal = [](Enemy &enemy, double time) -> void {
            enemy.position.x += sin(time) * 0.7;
        };
    }
}// namespace basilevs
#endif//BASILEVS_BEHAVIOURS_H
