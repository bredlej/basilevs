//
// Created by geoco on 22.06.2021.
//

#ifndef BASILEVS_ENEMY_H
#define BASILEVS_ENEMY_H
#include <world.h>
namespace behaviours {

    namespace bullet {
        constexpr auto fly_down = [](const double time, TWorld &, components::Sprite &sprite, components::Movement &movement) {
            movement.direction = movement.direction.Rotate(-5.0f);
            movement.position.y += movement.speed * time;
            movement.position = movement.position.Add(movement.direction);
        };
        using UpdateFunction = std::function<void(const double, TWorld &, components::Sprite &, components::Movement &)>;
    }// namespace bullet

    namespace enemy {
        constexpr auto shoot_bullet = [](TWorld &world, auto &emitter, const auto movement, const double time, const auto func) {
            constexpr auto emit_every_seconds = 1.0;
            if (emitter.last_emission > emit_every_seconds) {
                emitter.last_emission = 0.0;
                auto emitted_bullet = Blueprint(bullet::UpdateFunction(func));
                auto &sprite_component = get<components::Sprite>(emitted_bullet);
                sprite_component.texture = assets::TextureId::Bullet;
                sprite_component.frame_rect = raylib::Rectangle(0, 0, 8, 8);

                auto &bullet_movement = get<components::Movement>(emitted_bullet);
                bullet_movement.speed = 8.0;
                bullet_movement.position = movement.position;
                bullet_movement.position.x += bullet_movement.speed;
                bullet_movement.direction = raylib::Vector2(0.0, 1.0);
                world.enemy_bullets.add(emitted_bullet);
            }
            emitter.last_emission += time;
        };

        constexpr auto kEnemyNormal = [](const double time, TWorld &world, components::Sprite &sprite, components::Movement &movement, components::Activation &activation, components::TimeCounter &time_counter, components::Emission &emitter) {
            time_counter.elapsed_time += time;
            if (time_counter.elapsed_time > activation.activate_after_time) {
                activation.is_active = true;
            }
            if (activation.is_active) {
                shoot_bullet(world, emitter, movement, time, bullet::fly_down);
            }
        };
        using UpdateFunction = std::function<void(const double, TWorld &, components::Sprite &, components::Movement &, components::Activation &, components::TimeCounter &, components::Emission &)>;
    }// namespace enemy
}
#endif//BASILEVS_ENEMY_H
