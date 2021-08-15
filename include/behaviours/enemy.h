//
// Created by geoco on 22.06.2021.
//

#ifndef BASILEVS_ENEMY_H
#define BASILEVS_ENEMY_H
#include <boost/sml/sml.hpp>
#include <world.h>

namespace behaviours {

    namespace bullet {

        static constexpr auto bullet_sprite_update = [](components::Sprite &sprite) {
            sprite.frame_counter++;
            if (sprite.frame_counter >= (60 / sprite.frame_speed)) {
                sprite.frame_counter = 0;
                sprite.current_frame++;

                if (sprite.current_frame > sprite.amount_frames - 1) sprite.current_frame = 0;
                sprite.frame_rect.x = static_cast<float>(sprite.current_frame) * static_cast<float>(sprite.texture_width) / static_cast<float>(sprite.amount_frames);
            }
        };

        constexpr auto fly_towards_direction = [](const double time, TWorld &world, components::Sprite &sprite, components::Movement &movement, TWorld::BulletStateComponent &state, components::TimeCounter &time_counter) {
            movement.position.x += movement.direction.x * static_cast<float>(time) * movement.speed;
            movement.position.y += movement.direction.y * static_cast<float>(time) * movement.speed;
            bullet_sprite_update(sprite);

            const auto &player = world.player.get();
            const auto player_movement = get<components::Movement>(player->components);
            if (movement.position.CheckCollision(player_movement.position.Add({16, 16}), 4.0)) {
                movement.speed = 0.0f;
                state.state_machine.process_event(state::DestroyEvent());
            }
        };

        constexpr auto fly_and_rotate = [](const double time, TWorld &world, components::Sprite &sprite, components::Movement &movement, TWorld::BulletStateComponent &state, components::TimeCounter &time_counter) {
            movement.position.x += movement.direction.x * static_cast<float>(time) * movement.speed;
            movement.position.y += movement.direction.y * static_cast<float>(time) * movement.speed;
            time_counter.elapsed_time += time;
            if (time_counter.elapsed_time > 0.5) {
                movement.direction = movement.direction.Rotate(15).Normalize();
                movement.speed += 5.0f;
                time_counter.elapsed_time = 0;
            }
            bullet_sprite_update(sprite);

            const auto &player = world.player.get();
            const auto player_movement = get<components::Movement>(player->components);
            if (movement.position.CheckCollision(player_movement.position.Add({16, 16}), 4.0)) {
                movement.speed = 0.0f;
                state.state_machine.process_event(state::DestroyEvent());
            }
        };

        using EnemyBulletUpdateFunction = std::function<void(const double, TWorld &, components::Sprite &, components::Movement &, TWorld::BulletStateComponent &, components::TimeCounter &)>;
        struct BulletDefinition {
            assets::TextureId texture;
            uint32_t amount_frames;
            EnemyBulletUpdateFunction update_function;
        };
    }// namespace bullet

    namespace enemy {
        using UpdateFunction = std::function<void(const double, TWorld &, components::Sprite &, components::Movement &, components::Activation &, components::TimeCounter &, components::Emission &)>;
        struct EnemyDefinition {
            assets::TextureId texture;
            uint32_t amount_frames;
            bullet::BulletDefinition bullet;
            UpdateFunction behaviour;
        };
        constexpr auto tentacle_shoot_behaviour = [](TWorld &world, components::Emission &emitter, const components::Movement movement, const double time, const bullet::EnemyBulletUpdateFunction &bullet_function) {
            const auto &player = world.player.get();
            const auto player_movement = get<components::Movement>(player->components);
            constexpr auto emit_every_seconds = 1.0;
            if (emitter.last_emission > emit_every_seconds) {

                emitter.last_emission = 0.0;
                auto bullet_blueprint = Blueprint(bullet::EnemyBulletUpdateFunction(bullet_function));
                auto &bullet_sprite = get<components::Sprite>(bullet_blueprint);
                bullet_sprite.offset = raylib::Vector2{0.0f, 8.0f};
                bullet_sprite.texture = assets::TextureId::Bullet_Tentacle;
                bullet_sprite.frame_rect = raylib::Rectangle(0, 0, 8, 8);

                auto &bullet_movement = get<components::Movement>(bullet_blueprint);
                bullet_movement.speed = 40.0;
                bullet_movement.position = movement.position;
                bullet_movement.position = bullet_movement.position.Add(bullet_sprite.offset);
                bullet_movement.direction = player_movement.position.Subtract(bullet_movement.position.Add({6.0, 4.0})).Add({16.0, 16.0}).Normalize();
                bullet_sprite.rotation = player_movement.position.Add({16.0, 16.0}).Angle(bullet_movement.position.Add({4.0, 4.0}));

                auto &bullet_state = get<TWorld::BulletStateComponent>(bullet_blueprint);

                world.enemy_bullets.add(bullet_blueprint);
                world.sounds_queue.emplace_back(assets::SoundId::NormalBullet);
            }
            emitter.last_emission += time;
        };

        constexpr auto mosquito_shoot_behaviour = [](TWorld &world, components::Emission &emitter, const components::Movement movement, const double time, const bullet::EnemyBulletUpdateFunction &bullet_function) {
            const auto &player = world.player.get();
            const auto player_movement = get<components::Movement>(player->components);
            constexpr auto emit_every_seconds = 1.0;
            if (emitter.last_emission > emit_every_seconds) {

                emitter.last_emission = 0.0;
                for (int i = 1; i <= 12; i++) {
                    auto bullet_blueprint = Blueprint(bullet::EnemyBulletUpdateFunction(bullet_function));
                    auto &bullet_sprite = get<components::Sprite>(bullet_blueprint);
                    bullet_sprite.offset = raylib::Vector2{4.0f, 4.0f};
                    bullet_sprite.texture = assets::TextureId::Bullet_Mosquito;
                    bullet_sprite.current_frame = 0;
                    bullet_sprite.amount_frames = 6;
                    bullet_sprite.frame_rect = raylib::Rectangle(0, 0, 8, 8);
                    bullet_sprite.frame_speed = 12.0f;
                    bullet_sprite.texture_width = 48;
                    bullet_sprite.texture_height = 8;

                    auto &bullet_movement = get<components::Movement>(bullet_blueprint);
                    bullet_movement.speed = 15.0;
                    bullet_movement.position = movement.position;
                    bullet_movement.position = bullet_movement.position.Add(bullet_sprite.offset).Add(raylib::Vector2(0.0, 9.0).Rotate(180 / 3.14 * i));
                    bullet_movement.direction = raylib::Vector2(0.0, 1.0).Rotate(180 / 3.14 * i).Normalize();

                    auto &bullet_state = get<TWorld::BulletStateComponent>(bullet_blueprint);

                    world.enemy_bullets.add(bullet_blueprint);
                }

                world.sounds_queue.emplace_back(assets::SoundId::NormalBullet);
            }
            emitter.last_emission += time;
        };

        static constexpr auto frame_update = [](components::Sprite &sprite) {
            sprite.frame_counter++;

            if (sprite.frame_counter >= (60 / sprite.frame_speed)) {
                sprite.frame_counter = 0;
                sprite.current_frame++;

                if (sprite.current_frame > sprite.amount_frames - 1) sprite.current_frame = 0;

                sprite.frame_rect.x = static_cast<float>(sprite.current_frame) * static_cast<float>(sprite.texture_width) / static_cast<float>(sprite.amount_frames);
            }
        };

        constexpr auto kTentacleBehaviour = [](const double time, TWorld &world, components::Sprite &sprite, components::Movement &movement, components::Activation &activation, components::TimeCounter &time_counter, components::Emission &emitter) {
            frame_update(sprite);
            time_counter.elapsed_time += time;
            if (time_counter.elapsed_time > activation.activate_after_time) {
                activation.is_active = true;
            }
            if (activation.is_active) {
                tentacle_shoot_behaviour(world, emitter, movement, time, bullet::fly_towards_direction);
            }
        };

        constexpr auto kMosquitoBehaviour = [](const double time, TWorld &world, components::Sprite &sprite, components::Movement &movement, components::Activation &activation, components::TimeCounter &time_counter, components::Emission &emitter) {
            frame_update(sprite);
            time_counter.elapsed_time += time;
            if (time_counter.elapsed_time > activation.activate_after_time) {
                activation.is_active = true;
            }
            if (activation.is_active) {
                mosquito_shoot_behaviour(world, emitter, movement, time, bullet::fly_and_rotate);
            }
        };
        constexpr auto tentacle = []() -> EnemyDefinition {
            return {assets::TextureId::Tentacle, 9, {assets::TextureId::Bullet_Tentacle, 1, bullet::fly_towards_direction}, kTentacleBehaviour};
        };
        constexpr auto mosquito = []() -> EnemyDefinition {
            return {assets::TextureId::Mosquito, 11, {assets::TextureId::Bullet_Mosquito, 6, bullet::fly_towards_direction}, kMosquitoBehaviour};
        };

    }// namespace enemy
}// namespace behaviours
#endif//BASILEVS_ENEMY_H
