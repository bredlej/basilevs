//
// Created by geoco on 12.04.2021.
//

#ifndef BASILEVS_BEHAVIOURS_H
#define BASILEVS_BEHAVIOURS_H
#include <world.h>
#include <functional>

constexpr auto kFrameSpeed = 12;

namespace behaviours::player {

    constexpr auto frame_update = [] (components::Sprite &sprite) {
        sprite.frame_counter++;

        if (sprite.frame_counter >= (60 / kFrameSpeed)) {
            sprite.frame_counter = 0;
            sprite.current_frame++;

            if (sprite.current_frame > sprite.amount_frames- 1) sprite.current_frame = 0;

            sprite.frame_rect.x = static_cast<float>(sprite.current_frame) * static_cast<float>(sprite.texture_width) / static_cast<float>(sprite.amount_frames);
        }
    };
    constexpr auto kPlayerNormalBehaviour = [](const double time, TWorld &, components::Sprite & sprite, components::Movement &) {
        frame_update(sprite);
    };
    using PlayerUpdateFunction = std::function<void(const double, TWorld &, components::Sprite &, components::Movement &)>;
}
/*
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
 */
#endif//BASILEVS_BEHAVIOURS_H
