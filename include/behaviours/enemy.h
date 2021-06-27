//
// Created by geoco on 22.06.2021.
//

#ifndef BASILEVS_ENEMY_H
#define BASILEVS_ENEMY_H
#include <world.h>
namespace behaviours {

    namespace bullet {
        constexpr auto fly_towards_direction = [](const double time, TWorld &world, components::Sprite &sprite, components::Movement &movement) {
            movement.position.x += movement.direction.x * static_cast<float>(time) * movement.speed;
            movement.position.y += movement.direction.y * static_cast<float>(time) * movement.speed;
        };
        using UpdateFunction = std::function<void(const double, TWorld &, components::Sprite &, components::Movement &)>;
    }// namespace bullet

    namespace enemy {
        constexpr auto shoot_towards_player = [](TWorld &world, components::Emission &emitter, const components::Movement movement, const double time, const bullet::UpdateFunction &bullet_function) {
            const auto &player = world.player.get();
            const auto player_movement = get<components::Movement>(player->components);
            constexpr auto emit_every_seconds = 0.5;
            if (emitter.last_emission > emit_every_seconds) {
                emitter.last_emission = 0.0;
                auto emitted_bullet = Blueprint(bullet::UpdateFunction(bullet_function));
                auto &sprite_component = get<components::Sprite>(emitted_bullet);
                sprite_component.texture = assets::TextureId::Bullet;
                sprite_component.frame_rect = raylib::Rectangle(0, 0, 8, 8);

                auto &bullet_movement = get<components::Movement>(emitted_bullet);
                bullet_movement.speed = 40.0;
                bullet_movement.position.x = movement.position.x + 16;
                bullet_movement.position.y = movement.position.y + 16;
                bullet_movement.direction = player_movement.position.Subtract(bullet_movement.position.Add({4.0, 4.0})).Add({16.0, 16.0}).Normalize();
                sprite_component.rotation = player_movement.position.Add({16.0, 16.0}).Angle(bullet_movement.position.Add({4.0, 4.0}));
                world.enemy_bullets.add(emitted_bullet);
                world.sounds_queue.emplace_back(assets::SoundId::NormalBullet);
            }
            emitter.last_emission += time;
        };

        constexpr auto kEnemyNormal = [](const double time, TWorld &world, components::Sprite &sprite, components::Movement &movement, components::Activation &activation, components::TimeCounter &time_counter, components::Emission &emitter) {
            time_counter.elapsed_time += time;
            if (time_counter.elapsed_time > activation.activate_after_time) {
                activation.is_active = true;
            }
            if (activation.is_active) {
                shoot_towards_player(world, emitter, movement, time, bullet::fly_towards_direction);
            }
        };
        using UpdateFunction = std::function<void(const double, TWorld &, components::Sprite &, components::Movement &, components::Activation &, components::TimeCounter &, components::Emission &)>;
    }// namespace enemy
}// namespace behaviours
#endif//BASILEVS_ENEMY_H
