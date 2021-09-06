//
// Created by geoco on 22.06.2021.
//

#ifndef BASILEVS_ENEMY_H
#define BASILEVS_ENEMY_H
#include <boost/sml/sml.hpp>
#include <world.h>

/*
 * This header implements the behaviours of specific in-game enemies and bullets they shoot
 */
namespace behaviours {

    namespace bullet {
        /*
         * Declares a function which updates a single enemy bullet
         */
        using EnemyBulletUpdateFunction =
                std::function<void(
                        const double,
                        TWorld &,
                        components::Sprite &,
                        components::Movement &,
                        TWorld::BulletStateComponent &,
                        components::TimeCounter &,
                        components::Collision &,
                        components::Damage &)>;

        static constexpr auto bullet_sprite_update = [](components::Sprite &sprite) {
            sprite.fps_counter++;
            if (sprite.fps_counter >= (60 / sprite.fps_speed)) {
                sprite.fps_counter = 0;
                sprite.current_visible_frame++;

                if (sprite.current_visible_frame > sprite.amount_frames - 1) sprite.current_visible_frame = 0;
                sprite.frame_rect.x = static_cast<float>(sprite.current_visible_frame) * static_cast<float>(sprite.texture_width_px) / static_cast<float>(sprite.amount_frames);
            }
        };

        constexpr auto fly_towards_direction = [](const double time, TWorld &world, components::Sprite &sprite, components::Movement &movement, TWorld::BulletStateComponent &state, components::TimeCounter &time_counter, components::Collision &collision, components::Damage &damage) {
            movement.position.x += movement.direction.x * static_cast<float>(time) * movement.speed;
            movement.position.y += movement.direction.y * static_cast<float>(time) * movement.speed;
            bullet_sprite_update(sprite);
        };

        constexpr auto fly_and_rotate = [](const double time, TWorld &world, components::Sprite &sprite, components::Movement &movement, TWorld::BulletStateComponent &state, components::TimeCounter &time_counter, components::Collision &collision, components::Damage &damage) {
            movement.position.x += movement.direction.x * static_cast<float>(time) * movement.speed;
            movement.position.y += movement.direction.y * static_cast<float>(time) * movement.speed;
            time_counter.elapsed_seconds += time;
            if (time_counter.elapsed_seconds > 0.5) {
                movement.direction = movement.direction.Rotate(15).Normalize();
                movement.speed += 5.0f;
                time_counter.elapsed_seconds = 0;
            }
            bullet_sprite_update(sprite);
        };

        struct BulletDefinition {
            assets::TextureId texture;
            uint32_t amount_frames;
            EnemyBulletUpdateFunction update_function;
        };
    }// namespace bullet

    namespace enemy {
        /*
         * Declares a function which updates a single enemy object
         */
        using UpdateFunction =
                std::function<void(
                        const double,
                        TWorld &,
                        components::Sprite &,
                        components::Movement &,
                        components::Activation &,
                        components::TimeCounter &,
                        components::Emission &,
                        components::Collision &,
                        components::Health &,
                        TWorld::EnemyStateComponent &)>;

        struct EnemyDefinition {
            assets::TextureId texture;
            uint32_t amount_frames;
            bullet::BulletDefinition bullet;
            UpdateFunction behaviour;
            float health;
            raylib::Vector2 collision_center_offset;
            float collision_radius;
        };

        static constexpr auto frame_update = [](components::Sprite &sprite) {
            sprite.fps_counter++;

            if (sprite.fps_counter >= (60 / sprite.fps_speed)) {
                sprite.fps_counter = 0;
                sprite.current_visible_frame++;

                if (sprite.current_visible_frame > sprite.amount_frames - 1) sprite.current_visible_frame = 0;

                sprite.frame_rect.x = static_cast<float>(sprite.current_visible_frame) * static_cast<float>(sprite.texture_width_px) / static_cast<float>(sprite.amount_frames);
            }
        };

        /*
         *                  --- TENTACLE ---
         */
        namespace tentacle {
            static constexpr auto update_function = [](const double time, TWorld &world, components::Sprite &sprite, components::Movement &movement, components::Activation &activation, components::TimeCounter &time_counter, components::Emission &emitter, components::Collision &collision, components::Health &health, TWorld::EnemyStateComponent &state) {
                static constexpr auto tentacle_shoot_behaviour = [](TWorld &world, components::Emission &emitter, const components::Movement movement, const double time, const bullet::EnemyBulletUpdateFunction &bullet_function) {
                    const auto &player = world.player.get();
                    const auto player_movement = get<components::Movement>(player->components);
                    constexpr auto emit_every_seconds = 1.0;
                    if (emitter.last_emission_seconds > emit_every_seconds) {

                        emitter.last_emission_seconds = 0.0;
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
                        bullet_sprite.rotation_degrees = player_movement.position.Add({16.0, 16.0}).Angle(bullet_movement.position.Add({4.0, 4.0}));

                        auto &bullet_state = get<TWorld::BulletStateComponent>(bullet_blueprint);

                        auto &bullet_collision = get<components::Collision>(bullet_blueprint);
                        bullet_collision.bounds.center = raylib::Vector2{4.5f, 4.5f};
                        bullet_collision.bounds.radius = 4.0f;
                        bullet_collision.is_collidable = false;

                        auto &bullet_damage = get<components::Damage>(bullet_blueprint);
                        bullet_damage.value = 30.0f;

                        world.enemy_bullets.add(bullet_blueprint);
                        world.sounds_queue.emplace_back(assets::SoundId::NormalBullet);
                    }
                    emitter.last_emission_seconds += time;
                };
                static constexpr auto tentacle_state_handling = [](auto &state, auto &time_counter, auto &activation) {
                    /* State handling */

                    if (state.state_machine.is(state_handling::declarations::kInitState)) {
                        if (time_counter.elapsed_seconds > activation.activate_after_seconds) {
                            activation.is_active = true;
                            state.state_machine.process_event(state_handling::events::ArrivalEvent());
                        }
                    } else if (state.state_machine.is(state_handling::declarations::kArrivalState)) {
                        state.state_machine.process_event(state_handling::events::StartEvent());
                    }
                };
                frame_update(sprite);
                time_counter.elapsed_seconds += time;

                tentacle_state_handling(state, time_counter, activation);
                if (activation.is_active) {
                    tentacle_shoot_behaviour(world, emitter, movement, time, bullet::fly_towards_direction);
                }
            };

            static constexpr auto definition = []() -> EnemyDefinition {
                return {
                        assets::TextureId::Tentacle,
                        9,
                        {assets::TextureId::Bullet_Tentacle, 1, bullet::fly_towards_direction},
                        update_function,
                        40,
                        raylib::Vector2{8.0f, 8.0f},
                        8.0f};
            };
        }// namespace tentacle

        /*
         *                  --- MOSQUITO ---
         */
        namespace mosquito {
            static constexpr auto update_function = [](const double time, TWorld &world, components::Sprite &sprite, components::Movement &movement, components::Activation &activation, components::TimeCounter &time_counter, components::Emission &emitter, components::Collision &collision, components::Health &health, TWorld::EnemyStateComponent &state) {
                static constexpr auto mosquito_shoot_behaviour = [](TWorld &world, components::Emission &emitter, const components::Movement movement, const double time, const bullet::EnemyBulletUpdateFunction &bullet_function) {
                    const auto &player = world.player.get();
                    constexpr auto emit_every_seconds = 1.0;
                    if (emitter.last_emission_seconds > emit_every_seconds) {

                        emitter.last_emission_seconds = 0.0;
                        for (int i = 1; i <= 12; i++) {
                            auto bullet_blueprint = Blueprint(bullet::EnemyBulletUpdateFunction(bullet_function));
                            auto &bullet_sprite = get<components::Sprite>(bullet_blueprint);
                            bullet_sprite.offset = raylib::Vector2{4.0f, 4.0f};
                            bullet_sprite.texture = assets::TextureId::Bullet_Mosquito;
                            bullet_sprite.current_visible_frame = 0;
                            bullet_sprite.amount_frames = 6;
                            bullet_sprite.frame_rect = raylib::Rectangle(0, 0, 8, 8);
                            bullet_sprite.fps_speed = 12.0f;
                            bullet_sprite.texture_width_px = 48;
                            bullet_sprite.texture_height_px = 8;

                            auto &bullet_movement = get<components::Movement>(bullet_blueprint);
                            bullet_movement.speed = 15.0;
                            bullet_movement.position = movement.position;
                            bullet_movement.position = bullet_movement.position.Add(bullet_sprite.offset).Add(raylib::Vector2(0.0, 9.0).Rotate(180 / 3.14 * i));
                            bullet_movement.direction = raylib::Vector2(0.0, 1.0).Rotate(180 / 3.14 * i).Normalize();

                            auto &bullet_state = get<TWorld::BulletStateComponent>(bullet_blueprint);

                            auto &bullet_collision = get<components::Collision>(bullet_blueprint);
                            bullet_collision.bounds.center = raylib::Vector2{5.0f, 5.0f};
                            bullet_collision.bounds.radius = 2.0f;
                            bullet_collision.is_collidable = true;

                            world.enemy_bullets.add(bullet_blueprint);
                        }

                        world.sounds_queue.emplace_back(assets::SoundId::NormalBullet);
                    }
                    emitter.last_emission_seconds += time;
                };
                static constexpr auto mosquito_state_handling = [](auto &state, auto &time_counter, auto &activation) {
                    /* State handling */

                    if (state.state_machine.is(state_handling::declarations::kInitState)) {
                        if (time_counter.elapsed_seconds > activation.activate_after_seconds) {
                            activation.is_active = true;
                            state.state_machine.process_event(state_handling::events::ArrivalEvent());
                        }
                    } else if (state.state_machine.is(state_handling::declarations::kArrivalState)) {
                        state.state_machine.process_event(state_handling::events::StartEvent());
                    }
                };
                frame_update(sprite);
                time_counter.elapsed_seconds += time;

                mosquito_state_handling(state, time_counter, activation);
                if (activation.is_active) {
                    mosquito_shoot_behaviour(world, emitter, movement, time, bullet::fly_and_rotate);
                }
            };

            static constexpr auto definition = []() -> EnemyDefinition {
                return {
                        assets::TextureId::Mosquito,
                        11,
                        {assets::TextureId::Bullet_Mosquito, 6, bullet::fly_towards_direction},
                        update_function,
                        60,
                        raylib::Vector2{8.0f, 8.0f},
                        8.0f};
            };
        }// namespace mosquito
    }    // namespace enemy
}// namespace behaviours
#endif//BASILEVS_ENEMY_H
