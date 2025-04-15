//
// Created by geoco on 22.06.2021.
//

#ifndef BASILEVS_ENEMY_H
#define BASILEVS_ENEMY_H

#include "../../cmake-build-debug/_deps/raylib-build/raylib/include/raymath.h"

#include <boost/sml/sml.hpp>
#include <world.h>

/*
 * This header implements the behaviours of specific in-game enemies and bullets they shoot
 * Since enemies and their bullets must have a world they reside in, the components inside their respective UpdateFunctions must be the same as declared in the blueprint aliases (EnemyListType, BulletPool) in world.h
 */
namespace behaviours
{

    using UpdateFunctionType = std::function<void(double, entt::entity, entt::registry &)>;

    namespace bullet
    {

        static constexpr auto bullet_animation_update = [](components::Sprite &sprite)
        {
            sprite.fps_counter++;
            if (sprite.fps_counter >= (120 / sprite.fps_speed)) {
                sprite.fps_counter = 0;
                sprite.current_visible_frame++;

                if (sprite.current_visible_frame > sprite.amount_frames - 1) sprite.current_visible_frame = 0;
                sprite.frame_rect.x = static_cast<float>(sprite.current_visible_frame) * static_cast<float>(sprite.texture_width_px) / static_cast<float>(sprite.amount_frames);
            }
        };

        inline auto fly_towards_direction = [](const double time, const entt::entity entity, entt::registry &registry)
        {
            auto &movement = registry.get<components::Movement>(entity);
            auto &sprite = registry.get<components::Sprite>(entity);

            movement.position.x += movement.direction.x * static_cast<float>(time) * movement.speed;
            movement.position.y += movement.direction.y * static_cast<float>(time) * movement.speed;
            bullet_animation_update(sprite);
        };

        inline auto fly_and_rotate = [](const double time, const entt::entity entity, entt::registry &registry)
        {
            auto &movement = registry.get<components::Movement>(entity);
            auto &sprite = registry.get<components::Sprite>(entity);
            auto &[elapsed_seconds] = registry.get<components::TimeCounter>(entity);

            movement.position.x += movement.direction.x * static_cast<float>(time) * movement.speed;
            movement.position.y += movement.direction.y * static_cast<float>(time) * movement.speed;
            elapsed_seconds += time;
            if (elapsed_seconds > 0.5) {
                movement.direction = Vector2Normalize(Vector2Rotate(movement.direction, 15));
                movement.speed += 5.0f;
                elapsed_seconds = 0;
            }
            bullet_animation_update(sprite);
        };

        struct BulletDefinition {
            assets::TextureId texture;
            uint32_t amount_frames;
            UpdateFunctionType update_function;
        };
    }// namespace bullet

    namespace enemy
    {
        static constexpr auto move_towards_path = [](const double time, components::Movement &movement, components::MovementPath &movementPath)
        {
            if (!movementPath.points.empty()) {
                const auto nextPoint = movementPath.points.at(0);
                movement.position = Vector2MoveTowards(movement.position, nextPoint, movement.speed * time);
                if (Vector2Distance(nextPoint, movement.position) < 1.1) { movementPath.points.pop_front(); }
            }
        };

        static constexpr auto common_state_handling = [](auto &state, auto &time_counter, const auto time, components::Movement &movement, components::MovementPath &movement_path, auto &activation, components::Sprite &sprite)
        {
            /* State handling */

            if (state.state_machine.is(state_handling::declarations::kInitState)) {
                if (time_counter.elapsed_seconds > activation.activate_after_seconds) {
                    activation.is_active = true;
                    state.state_machine.process_event(state_handling::events::ArrivalEvent());
                }
            } else if (state.state_machine.is(state_handling::declarations::kArrivalState)) {
                move_towards_path(time, movement, movement_path);
                if (movement_path.points.empty()) { state.state_machine.process_event(state_handling::events::StartEvent()); }
            } else if (state.state_machine.is(state_handling::declarations::kArrivalState)) {} else if (state.state_machine.is(state_handling::declarations::kTakingDamageState)) {
                sprite.current_state = components::StateEnum::DESTROYED;
                sprite.current_visible_frame = sprite.state_animations[components::StateEnum::DESTROYED].begin_frame;
                state.state_machine.process_event(state_handling::events::KillEvent());
                activation.is_active = false;
            } else if (state.state_machine.is(state_handling::declarations::kDeadState)) { if (sprite.state_animations[sprite.current_state].has_ended) { state.state_machine.process_event(state_handling::events::DestroyEvent()); } }
        };

        struct EnemyDefinition {
            assets::TextureId texture;
            uint32_t amount_frames;
            bullet::BulletDefinition bullet;
            components::UpdateFunction behaviour;
            float health;
            Vector2 collision_center_offset;
            float collision_radius;
            std::deque<Vector2> path;
            float speed;
            std::unordered_map<components::StateEnum, components::animation> animations;
        };

        static constexpr auto animate_state = [](components::Sprite &sprite)
        {
            sprite.fps_counter++;

            if (sprite.fps_counter >= (120 / sprite.fps_speed)) {
                sprite.fps_counter = 0;
                sprite.current_visible_frame++;
                auto &current_animation = sprite.state_animations[sprite.current_state];
                if (sprite.current_visible_frame > current_animation.end_frame - 1) { if (current_animation.is_repeating) { sprite.current_visible_frame = current_animation.begin_frame; } else { current_animation.has_ended = true; } }

                sprite.frame_rect.x = static_cast<float>(sprite.current_visible_frame) * static_cast<float>(sprite.texture_width_px) / static_cast<float>(sprite.amount_frames);
            }
        };

        /*
         *                  --- TENTACLE ---
         */
        namespace tentacle
        {
            static constexpr auto update_function =
                    [](
                    const double time,
                    const entt::entity entity,
                    entt::registry &registry)
            {
                static auto tentacle_shoot_behaviour = [&registry, &time](components::Emission &emitter, const components::Movement &movement, const UpdateFunctionType &bullet_function)
                {
                    const auto player = registry.ctx().get<components::Player>().entity;
                    const auto player_movement = registry.get<components::Movement>(player);

                    if (constexpr auto emit_every_seconds = 1.0; emitter.last_emission_seconds > emit_every_seconds) {
                        emitter.last_emission_seconds = 0.0;

                        components::Sprite bullet_sprite;
                        bullet_sprite.offset = Vector2{0.0f, 8.0f};
                        bullet_sprite.texture = assets::TextureId::Bullet_Tentacle;
                        bullet_sprite.frame_rect = Rectangle(0, 0, 8, 8);

                        components::Movement bullet_movement;
                        bullet_movement.speed = 40.0;
                        bullet_movement.position = movement.position;
                        bullet_movement.position = Vector2Add(bullet_movement.position, bullet_sprite.offset);
                        bullet_movement.direction = Vector2Normalize(Vector2Add(player_movement.position, Vector2Add(player_movement.position, Vector2Subtract(player_movement.position, Vector2Add(bullet_movement.position, {6.0, 4.0})))));

                        components::Collision bullet_collision;
                        bullet_collision.bounds.center = Vector2{4.5f, 4.5f};
                        bullet_collision.bounds.radius = 4.0f;
                        bullet_collision.is_collidable = false;

                        components::Damage bullet_damage;
                        bullet_damage.value = 30.0f;

                        BlueprintEntt(registry).builder()
                                               .with<TWorld::BulletStateComponent>()
                                               .with<components::UpdateFunction>(bullet_function)
                                               .with<components::Sprite>(bullet_sprite)
                                               .with<components::Movement>(bullet_movement)
                                               .with<components::Collision>(bullet_collision)
                                               .with<components::Damage>(bullet_damage)
                                               .with<components::EnemyBullet>();


                        Vector2Angle(Vector2Add(player_movement.position, {16.0, 16.0}), Vector2Add(bullet_movement.position, {4.0, 4.0}));

                        //world.sounds_queue.emplace_back(assets::SoundId::NormalBullet);
                    }
                    emitter.last_emission_seconds += time;
                };
                components::Sprite &sprite = registry.get<components::Sprite>(entity);
                animate_state(sprite);
                components::TimeCounter &time_counter = registry.get<components::TimeCounter>(entity);
                time_counter.elapsed_seconds += time;

                TWorld::EnemyStateComponent &state = registry.get<TWorld::EnemyStateComponent>(entity);
                components::Movement &movement = registry.get<components::Movement>(entity);
                components::MovementPath &movement_path = registry.get<components::MovementPath>(entity);
                components::Activation &activation = registry.get<components::Activation>(entity);
                components::Emission &emitter = registry.get<components::Emission>(entity);

                common_state_handling(state, time_counter, time, movement, movement_path, activation, sprite);
                if (activation.is_active) { tentacle_shoot_behaviour(emitter, movement, bullet::fly_towards_direction); }
            };

            static constexpr auto definition = [](std::deque<Vector2> &&movement_path) -> EnemyDefinition
            {
                return {
                        assets::TextureId::Tentacle,
                        18,
                        {assets::TextureId::Bullet_Tentacle, 1, bullet::fly_towards_direction},
                        components::UpdateFunction(update_function),
                        40,
                        Vector2{8.0f, 8.0f},
                        8.0f,
                        movement_path,
                        10.0f,
                        {
                                {components::StateEnum::IDLE, components::animation{0, 8, false, true}},
                                {components::StateEnum::DESTROYED, components::animation{9, 17, false, false}},
                        }};
            };
        }// namespace tentacle

        /*
         *                  --- MOSQUITO ---
         */
        namespace mosquito
        {
            static constexpr auto update_function =
                    [](
                    const double time,
                    const entt::entity entity,
                    entt::registry &registry)
            {
                static auto mosquito_shoot_behaviour = [&registry, &time](components::Emission &emitter, const components::Movement &mosquito_movement, const UpdateFunctionType &bullet_function)
                {
                    if (constexpr auto emit_every_seconds = 1.0; emitter.last_emission_seconds > emit_every_seconds) {
                        emitter.last_emission_seconds = 0.0;
                        for (int i = 1; i <= 12; i++) {
                            components::Sprite bullet_sprite;
                            bullet_sprite.offset = Vector2{4.0f, 4.0f};
                            bullet_sprite.texture = assets::TextureId::Bullet_Mosquito;
                            bullet_sprite.current_visible_frame = 0;
                            bullet_sprite.amount_frames = 6;
                            bullet_sprite.frame_rect = Rectangle(0, 0, 8, 8);
                            bullet_sprite.fps_speed = 12.0f;
                            bullet_sprite.texture_width_px = 48;
                            bullet_sprite.texture_height_px = 8;

                            components::Movement bullet_movement;
                            bullet_movement.speed = 15.0;
                            bullet_movement.position = mosquito_movement.position;
                            // fix above with Vector2Add
                            bullet_movement.position = Vector2Add(bullet_movement.position, bullet_sprite.offset);
                            bullet_movement.position = Vector2Add(bullet_movement.position, Vector2{0.0f, 9.0f});
                            bullet_movement.position = Vector2Rotate(bullet_movement.position, 180 / 3.14 * i);

                            bullet_movement.direction = Vector2{0.0f, 1.0f};
                            bullet_movement.direction = Vector2Normalize(Vector2Rotate(bullet_movement.direction, 180 / 3.14 * i));

                            components::Collision bullet_collision;

                            bullet_collision.bounds.center = Vector2{5.0f, 5.0f};
                            bullet_collision.bounds.radius = 2.0f;
                            bullet_collision.is_collidable = true;

                            BlueprintEntt(registry).builder()
                                                   .with<TWorld::BulletStateComponent>()
                                                   .with<components::UpdateFunction>(bullet_function)
                                                   .with<components::Sprite>(bullet_sprite)
                                                   .with<components::Movement>(bullet_movement)
                                                   .with<components::Collision>(bullet_collision)
                                                   .with<components::TimeCounter>()
                                                   .with<components::EnemyBullet>();
                        }

                        //world.sounds_queue.emplace_back(assets::SoundId::NormalBullet);
                    }
                    emitter.last_emission_seconds += time;
                };

                components::Sprite &sprite = registry.get<components::Sprite>(entity);
                animate_state(sprite);
                components::TimeCounter &time_counter = registry.get<components::TimeCounter>(entity);
                time_counter.elapsed_seconds += time;

                TWorld::EnemyStateComponent &state = registry.get<TWorld::EnemyStateComponent>(entity);
                components::Movement &movement = registry.get<components::Movement>(entity);
                components::MovementPath &movement_path = registry.get<components::MovementPath>(entity);
                components::Activation &activation = registry.get<components::Activation>(entity);
                components::Emission &emitter = registry.get<components::Emission>(entity);
                common_state_handling(state, time_counter, time, movement, movement_path, activation, sprite);
                if (activation.is_active) { mosquito_shoot_behaviour(emitter, movement, bullet::fly_and_rotate); }
            };

            static constexpr auto definition = [](std::deque<Vector2> &&movement_path) -> EnemyDefinition
            {
                return {
                        assets::TextureId::Mosquito,
                        17,
                        {assets::TextureId::Bullet_Mosquito, 6, bullet::fly_and_rotate},
                        components::UpdateFunction(update_function),
                        60,
                        Vector2{8.0f, 8.0f},
                        8.0f,
                        movement_path,
                        10.0f,
                        {
                                {components::StateEnum::IDLE, {0, 10, false, true}},
                                {components::StateEnum::DESTROYED, {11, 16, false, false}},
                        }};
            };
        }// namespace mosquito
        static std::unordered_map<std::string, std::function<EnemyDefinition(std::deque<Vector2>)>> const definitions = {
                {"MOSQUITO", mosquito::definition},
                {"TENTACLE", tentacle::definition}};
    }// namespace enemy
}// namespace behaviours
#endif//BASILEVS_ENEMY_H