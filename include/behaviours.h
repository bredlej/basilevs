//
// Created by geoco on 12.04.2021.
//

#ifndef BASILEVS_BEHAVIOURS_H
#define BASILEVS_BEHAVIOURS_H
#include <functional>
#include <world.h>

constexpr auto kFrameSpeed = 12;

namespace behaviours {
    namespace player {
        constexpr auto frame_update = [](components::Sprite &sprite) {
            sprite.frame_counter++;

            if (sprite.frame_counter >= (60 / kFrameSpeed)) {
                sprite.frame_counter = 0;
                sprite.current_frame++;

                if (sprite.current_frame > sprite.amount_frames - 1) sprite.current_frame = 0;

                sprite.frame_rect.x = static_cast<float>(sprite.current_frame) * static_cast<float>(sprite.texture_width) / static_cast<float>(sprite.amount_frames);
            }
        };
        constexpr auto kPlayerNormalBehaviour = [](const double time, TWorld &, components::Sprite &sprite, components::Movement &) {
            frame_update(sprite);
        };
        using UpdateFunction = std::function<void(const double, TWorld &, components::Sprite &, components::Movement &)>;
    }// namespace player
    namespace bullet {
        constexpr auto fly_down = [](const double time, TWorld &, components::Sprite &sprite, components::Movement &movement) {
            movement.direction = movement.direction.Rotate(-5.0f);
            movement.position.y += 20.0 * time;
            movement.position = movement.position.Add(movement.direction);
        };
        using UpdateFunction = std::function<void(const double, TWorld &, components::Sprite &, components::Movement &)>;
    }// namespace bullet
    constexpr auto emit = [](TWorld &world, auto &emitter, const auto movement, const double time, const auto func) {
        constexpr auto emit_every_seconds = 1.0;
        if (emitter.last_emission > emit_every_seconds) {
            emitter.last_emission = 0.0;
            auto emitted_bullet = Blueprint(bullet::UpdateFunction(func));
            auto &sprite_component = get<components::Sprite>(emitted_bullet);
            sprite_component.texture = assets::TextureId::Bullet;
            sprite_component.frame_rect = raylib::Rectangle(0, 0, 8, 8);
            auto &movement_component = get<components::Movement>(emitted_bullet);
            movement_component.position = movement.position;
            movement_component.position.x += 8;
            movement_component.direction = raylib::Vector2(0.0, 1.0);
            world.enemy_bullets.add(emitted_bullet);
        }
        emitter.last_emission += time;
    };
    namespace enemy {
        constexpr auto kEnemyNormal = [](const double time, TWorld &world, components::Sprite &sprite, components::Movement &movement, components::Activation &activation, components::TimeCounter &time_counter, components::Emission &emitter) {
            time_counter.elapsed_time += time;
            if (time_counter.elapsed_time > activation.activate_after_time) {
                activation.is_active = true;
            }
            if (activation.is_active) {
                emit(world, emitter, movement, time, bullet::fly_down);
            }
        };
        using UpdateFunction = std::function<void(const double, TWorld &, components::Sprite &, components::Movement &, components::Activation &, components::TimeCounter &, components::Emission &)>;
    }// namespace enemy
    namespace background {
        using UpdateFunction = std::function<void(const double, TWorld &, components::Sprite &, components::Movement &)>;
        constexpr auto level1_background_update = [](const double time, TWorld &, components::Sprite &sprite, components::Movement &) {
            sprite.frame_counter += 1;
            if (sprite.frame_rect.y > 0) {
                if (sprite.frame_counter >= (60 / kFrameSpeed)) {
                    sprite.frame_counter = 0;
                    sprite.frame_rect.y -= 1 - time;
                }
            }
        };
    }// namespace background
}// namespace behaviours
#endif//BASILEVS_BEHAVIOURS_H
