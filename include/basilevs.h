//
// Created by geoco on 09.04.2021.
//

#ifndef BASILEVS_CORE_H
#define BASILEVS_CORE_H

#include <behaviours/background.h>
#include <behaviours/enemy.h>
#include <behaviours/player.h>
#include <chrono>
#include <concepts>
#include <config.h>
#include <functional>
#include <iostream>
#include <ranges>
#include <raylib-cpp.hpp>
#include <raylib-cpp/include/Functions.hpp>
#include <raylib-cpp/include/Vector2.hpp>
#include <raylib.h>
#include <utility>
#include <world.h>

namespace basilevs
{
    using namespace components;
    namespace initialization
    {
        enum class AnimationDirection
        {
            Vertical,
            Horizontal
        };

        static void setup_sprite(Sprite &sprite_component, const std::vector<Texture2D> &textures, const assets::TextureId texture_id, const uint32_t amount_frames, const AnimationDirection animation_direction)
        {
            int texture_width = textures[static_cast<int>(texture_id)].width;
            int texture_height = textures[static_cast<int>(texture_id)].height;
            sprite_component.current_visible_frame = std::rand() % amount_frames;
            sprite_component.texture = texture_id;
            sprite_component.amount_frames = amount_frames;
            sprite_component.texture_width_px = texture_width;
            sprite_component.texture_height_px = texture_height;
            sprite_component.fps_speed = 12;
            if (AnimationDirection::Horizontal == animation_direction) {
                sprite_component.frame_rect = {0.0f, 0.0f, static_cast<float>(texture_width) / static_cast<float>(sprite_component.amount_frames), static_cast<float>(texture_height)};
            } else {
                sprite_component.frame_rect = {0.0, static_cast<float>(texture_height) - (static_cast<float>(texture_height) / static_cast<float>(amount_frames)), static_cast<float>(texture_width), static_cast<float>(texture_height) / static_cast<float>(amount_frames)};
            }
        }

        static void setup_enemy_sprite(Sprite &sprite_component, const std::vector<Texture2D> &textures, const behaviours::enemy::EnemyDefinition &enemy_definition)
        {
            setup_sprite(sprite_component, textures, enemy_definition.texture, enemy_definition.amount_frames, AnimationDirection::Horizontal);
            sprite_component.current_state = StateEnum::IDLE;
            sprite_component.state_animations = enemy_definition.animations;
        }

        static void setup_movement(Movement &movement, const float x, const float y, const float speed)
        {
            movement.position = raylib::Vector2{x, y};
            movement.speed = speed;
        }

        static void setup_activation(Activation &activation, const double time)
        {
            activation.activate_after_seconds = time;
            activation.is_active = false;
        }

        static void setup_collision(Collision &collision, const float radius, const raylib::Vector2 &center)
        {
            collision.bounds.radius = radius;
            collision.bounds.center = center;
            collision.is_collidable = true;
        }

        static TWorld::PlayerType create_player(const std::vector<Texture2D> &textures)
        {
            TWorld::PlayerType player = Blueprint(behaviours::player::UpdateFunction(behaviours::player::default_behaviour));
            setup_sprite(get<Sprite>(player), textures, assets::TextureId::Player, 7, AnimationDirection::Horizontal);
            setup_movement(get<Movement>(player), 70.0f, 100.0f, 50.0f);
            setup_collision(get<Collision>(player), 3.0f, raylib::Vector2{17.0f, 18.0f});

            return player;
        }

        static TWorld::EnemyType create_enemy(const double seconds_until_spawns, const Vector2 &position, const behaviours::enemy::EnemyDefinition &enemy_definition)
        {
            TWorld::EnemyType enemy = Blueprint(behaviours::enemy::UpdateFunction(enemy_definition.behaviour));
            get<Movement>(enemy).position = position;
            get<Movement>(enemy).speed = enemy_definition.speed;
            get<TimeCounter>(enemy).elapsed_seconds = 0.0;
            get<Emission>(enemy).last_emission_seconds = 0.0f;
            get<MovementPath>(enemy).points = enemy_definition.path;
            get<Health>(enemy).hp = enemy_definition.health;
            setup_activation(get<Activation>(enemy), seconds_until_spawns);
            setup_collision(get<Collision>(enemy), enemy_definition.collision_radius, enemy_definition.collision_center_offset);

            return enemy;
        }

        static TWorld::EnemyType create_enemy_with_sprite(const double seconds_until_spawns, const std::vector<Texture2D> &textures, const Vector2 &position, const behaviours::enemy::EnemyDefinition &enemy_definition)
        {
            TWorld::EnemyType enemy = create_enemy(seconds_until_spawns, position, enemy_definition);
            setup_enemy_sprite(get<Sprite>(enemy), textures, enemy_definition);

            return enemy;
        }

        static TWorld::BackgroundType create_background(const std::vector<Texture2D> &textures)
        {
            TWorld::BackgroundType background = Blueprint(behaviours::background::UpdateFunction(behaviours::background::level1_background_update));
            setup_sprite(get<Sprite>(background), textures, assets::TextureId::Background_Level_1, 6, AnimationDirection::Vertical);

            return background;
        }
    }// namespace initialization

    namespace game_state
    {
        static void update_world(const auto time_since_last_update, TWorld &world)
        {
            world.background->update(time_since_last_update.count(), world);
            world.player->update(time_since_last_update.count(), world);
            world.enemies->update(time_since_last_update.count(), world);
            world.enemy_bullets.update(time_since_last_update.count(), world);
            world.player_bullets.update(time_since_last_update.count(), world);
        }
    }// namespace game_state

    namespace rendering
    {
        static void render_player(raylib::RenderTexture &render_target, const TWorld &world, const std::vector<Texture2D> &textures)
        {
            const TWorld::PlayerType *player = world.player.get();

            const auto &sprite_component = std::get<Sprite>(player->components);
            const auto &movement_component = std::get<Movement>(player->components);
            const Texture &texture = textures[static_cast<int>(sprite_component.texture)];

            DrawTextureRec(texture, sprite_component.frame_rect, movement_component.position, WHITE);
        }

        static bool is_render_allowed_for_state(const TWorld::EnemyStateComponent state)
        {
            return !state.state_machine.is(boost::sml::X) && !state.state_machine.is(state_handling::declarations::kInitState);
        }

        static void render_enemy(const std::vector<Texture> &textures, const Sprite &sprite, const Movement &movement, const TWorld::EnemyStateComponent &state)
        {
            if (is_render_allowed_for_state(state)) {
                DrawTextureRec(textures[static_cast<int>(sprite.texture)], sprite.frame_rect, movement.position, WHITE);
            }
        }

        static void render_enemies(raylib::RenderTexture &render_target, const TWorld &world, const std::vector<Texture2D> &textures)
        {
            const auto &enemy_components = world.enemies->components;
            for (size_t enemy_idx = 0; enemy_idx < world.enemies->functions.size(); enemy_idx++) {
                render_enemy(textures,
                             std::get<std::vector<Sprite>>(enemy_components)[enemy_idx],
                             std::get<std::vector<Movement>>(enemy_components)[enemy_idx],
                             std::get<std::vector<TWorld::EnemyStateComponent>>(enemy_components)[enemy_idx]);
            }
        }

        static void render_enemy_bullets(const TWorld &world, const std::vector<Texture> &textures, const std::vector<Movement> &movements, const std::vector<Sprite> &sprites)
        {
            for (std::size_t i = 0; i < world.enemy_bullets.first_available_index; i++) {
                DrawTextureRec(textures[static_cast<int>(sprites[i].texture)],
                               sprites[i].frame_rect,
                               movements[i].position,
                               WHITE);
            }
        }

        static void render_player_bullets(const TWorld &world, const std::vector<Texture> &textures, const std::vector<Movement> &movements, const std::vector<Sprite> &sprites)
        {
            for (std::size_t i = 0; i < world.player_bullets.first_available_index; i++) {
                DrawTextureEx(textures[static_cast<int>(sprites[i].texture)],
                              movements[i].position,
                              sprites[i].rotation_degrees,
                              1.0f,
                              WHITE);
            }
        }

        static void render_bullets(raylib::RenderTexture &render_target, const TWorld &world, const std::vector<Texture2D> &textures)
        {
            const auto &enemy_components = world.enemy_bullets.components;
            render_enemy_bullets(world,
                                 textures,
                                 std::get<std::vector<Movement>>(enemy_components),
                                 std::get<std::vector<Sprite>>(enemy_components));

            const auto &player_components = world.player_bullets.components;
            render_player_bullets(world,
                                  textures,
                                  std::get<std::vector<Movement>>(player_components),
                                  std::get<std::vector<Sprite>>(player_components));
        }

        static void render_background(raylib::RenderTexture &render_target, const TWorld &world, const std::vector<Texture2D> &textures)
        {
            const TWorld::BackgroundType *background = world.background.get();
            const auto &sprite_component = std::get<Sprite>(background->components);
            const auto &texture = textures[static_cast<int>(sprite_component.texture)];

            DrawTextureRec(texture, sprite_component.frame_rect, {0, 0}, GRAY);
        }

        /*
         * Renders the current game state into a texture.
         *
         * The textures dimension is declared in config.h, in this case something small like 160x144px.
         * After all contents are rendered, the texture itself will be rendered upscaled to the users screen dimensions in another function below.
         */
        static void render_to_texture(raylib::RenderTexture &render_target, const TWorld &world, const std::vector<Texture2D> &textures)
        {
            BeginTextureMode(render_target);
            ClearBackground(config::colors::kBackground);
            render_background(render_target, world, textures);
            render_player(render_target, world, textures);
            render_enemies(render_target, world, textures);
            render_bullets(render_target, world, textures);
            EndTextureMode();
        }

        /*
         * This function renders the games current frame upscaled to actual screen dimensions, along with some UI elements
         */
        static void render_to_screen(raylib::RenderTexture &render_target, const TWorld &world)
        {
            DrawTexturePro(render_target.texture,
                           Rectangle{0.0f, 0.0f, (float) render_target.texture.width, (float) -render_target.texture.height},
                           raylib::Rectangle{0.0f, 0.0f, static_cast<float>(config::kScreenWidth), static_cast<float>(config::kScreenHeight)},
                           Vector2{0, 0},
                           0.0f,
                           WHITE);
            raylib::DrawText(std::to_string(world.enemy_bullets.first_available_index), config::kScreenWidth - 60, 60, 30, ORANGE);
            raylib::DrawText(std::to_string(world.player_bullets.first_available_index), config::kScreenWidth - 60, 95, 30, ORANGE);
            DrawFPS(5, 5);
        }
    }// namespace rendering

    namespace collision_checking
    {
        /*
         * Helper struct which holds references to bullet components used in collision checking
         */
        struct CollisionCheckBulletComponents {
            std::vector<Movement> &movements;
            std::vector<Collision> &collisions;
            std::vector<TWorld::BulletStateComponent> &states;
            std::vector<Damage> &damages;
        };

        static CollisionCheckBulletComponents retrieve_components_for_bullets(auto &components)
        {
            return {
                    std::get<std::vector<Movement>>(components),
                    std::get<std::vector<Collision>>(components),
                    std::get<std::vector<TWorld::BulletStateComponent>>(components),
                    std::get<std::vector<Damage>>(components)};
        }

        static bool is_enemy_collidable(const size_t enemy_idx, const auto &enemy_components)
        {
            const auto &enemy_activation = std::get<std::vector<Activation>>(enemy_components);
            return enemy_activation[enemy_idx].is_active;
        }

        static void destroy_bullet(TWorld::BulletStateComponent &bullet_state)
        {
            bullet_state.state_machine.process_event(state_handling::events::DestroyEvent());
        }

        static void handle_collision(const size_t enemy_idx, auto &enemy_components, const size_t bullet_idx, const auto &bullet_components)
        {
            std::get<std::vector<TWorld::EnemyStateComponent>>(enemy_components)[enemy_idx]
                    .state_machine
                    .process_event(state_handling::events::DamageEvent());

            destroy_bullet(bullet_components.states[bullet_idx]);
        }

        static raylib::Vector2 get_collision_center(const Movement &movement, const Collision &collision) {
            return movement.position.Add(collision.bounds.center);
        }

        static float get_radius(const Collision &collision) {
            return collision.bounds.radius;
        }
        /*
         * Handle enemies colliding with bullets shot by the player
         */
        static void player_bullets_with_enemies(TWorld &world)
        {
            auto &enemy_components = world.enemies->components;
            const std::vector<Collision> &enemy_collisions = std::get<std::vector<Collision>>(enemy_components);
            const std::vector<Movement> &enemy_movements = std::get<std::vector<Movement>>(enemy_components);

            const CollisionCheckBulletComponents &bullet_components = retrieve_components_for_bullets(world.player_bullets.components);

            for (std::size_t bullet_idx = 0; bullet_idx < world.player_bullets.first_available_index; bullet_idx++) {
                const Movement &bullet_movement = bullet_components.movements[bullet_idx];
                const Collision &bullet_collision = bullet_components.collisions[bullet_idx];

                const raylib::Vector2 &bullet_center = get_collision_center(bullet_movement, bullet_collision);
                const float &bullet_radius = get_radius(bullet_collision);

                for (std::size_t enemy_idx = 0; enemy_idx < world.enemies->functions.size(); enemy_idx++) {
                    if (is_enemy_collidable(enemy_idx, enemy_components)) {
                        const raylib::Vector2 &enemy_center = get_collision_center(enemy_movements[enemy_idx], enemy_collisions[enemy_idx]);
                        const float &enemy_radius = get_radius(enemy_collisions[enemy_idx]);
                        if (CheckCollisionCircles(bullet_center, bullet_radius, enemy_center, enemy_radius)) {
                            handle_collision(enemy_idx, enemy_components, bullet_idx, bullet_components);
                        }
                    }
                }
            }
        }

        /*
         * Handle player bullets colliding with other bullets.
         * Intended for the case when enemies are shooting swarms of bullets which can be destroyed when being shot at.
         */
        static void player_bullets_with_enemy_bullets(TWorld &world)
        {
            const CollisionCheckBulletComponents &player_bullets = retrieve_components_for_bullets(world.player_bullets.components);
            const CollisionCheckBulletComponents &enemy_bullets = retrieve_components_for_bullets(world.enemy_bullets.components);

            for (std::size_t player_bullet_idx = 0; player_bullet_idx < world.player_bullets.first_available_index; player_bullet_idx++) {

                const raylib::Vector2 player_collision_center = get_collision_center(player_bullets.movements[player_bullet_idx], player_bullets.collisions[player_bullet_idx]);

                for (std::size_t enemy_bullet_idx = 0; enemy_bullet_idx < world.enemy_bullets.first_available_index; enemy_bullet_idx++) {
                    if (!enemy_bullets.collisions[enemy_bullet_idx].is_collidable) {
                        continue;
                    }
                    const raylib::Vector2 enemy_collision_center = get_collision_center(enemy_bullets.movements[enemy_bullet_idx], enemy_bullets.collisions[enemy_bullet_idx]);

                    if (CheckCollisionCircles(player_collision_center, player_bullets.collisions[player_bullet_idx].bounds.radius, enemy_collision_center, enemy_bullets.collisions[enemy_bullet_idx].bounds.radius)) {
                        destroy_bullet(player_bullets.states[player_bullet_idx]);
                        destroy_bullet(enemy_bullets.states[enemy_bullet_idx]);
                    }
                }
            }
        }

        static void damage_player(Health &player_health, const float damage)
        {
            player_health.hp -= damage;
        }

        /*
         * Handle player colliding with enemy bullets
         */
        static void enemy_bullets_with_player(auto &world)
        {
            const auto &player_movement = std::get<Movement>(world.player.get()->components);
            const auto &player_collision = std::get<Collision>(world.player.get()->components);
            auto &player_health = std::get<Health>(world.player.get()->components);
            const raylib::Vector2 &player_collision_center = player_movement.position.Add(player_collision.bounds.center);

            const auto &enemy_bullets = retrieve_components_for_bullets(world.enemy_bullets.components);

            for (std::size_t bullet_idx = 0; bullet_idx < world.enemy_bullets.first_available_index; bullet_idx++) {
                const raylib::Vector2 &collision_center = get_collision_center(enemy_bullets.movements[bullet_idx], enemy_bullets.collisions[bullet_idx]);

                if (CheckCollisionCircles(collision_center, enemy_bullets.collisions[bullet_idx].bounds.radius, player_collision_center, player_collision.bounds.radius)) {
                    destroy_bullet(enemy_bullets.states[bullet_idx]);
                    damage_player(player_health, enemy_bullets.damages[bullet_idx].value);
                }
            }
        }

        /*
         * Here we check if the player, enemies and bullets collide with each other and do some logic if they do.
         */
        static void collision_checks(TWorld &world)
        {
            player_bullets_with_enemies(world);
            player_bullets_with_enemy_bullets(world);
            enemy_bullets_with_player(world);
        }
    }// namespace collision_checking

    namespace memory
    {
        static bool is_bullet_outside_frame(const size_t index, const auto &bullet_components, const TWorld &world)
        {
            return !raylib::Rectangle(std::get<std::vector<Movement>>(bullet_components)[index - 1].position,
                                      std::get<std::vector<Sprite>>(bullet_components)[index - 1].bounds)
                            .CheckCollision(world.frame_bounds);
        }

        /*
         * Deactivates bullets which are outside the visible frame by a given margin.
         * Those bullets go into a "destroyed" state.
         */
        static void destroy_bullets_outside_frame(const TWorld &world, auto &bullet_pool)
        {
            for (std::size_t bullet_idx = bullet_pool.first_available_index; bullet_idx > 0; bullet_idx--) {
                if (is_bullet_outside_frame(bullet_idx, bullet_pool.components, world)) {
                    std::get<std::vector<TWorld::BulletStateComponent>>(bullet_pool.components)[bullet_idx - 1]
                            .state_machine
                            .process_event(state_handling::events::DestroyEvent());
                }
            }
        }

        static void remove_destroyed_bullets(auto &pool)
        {
            for (std::size_t index = pool.first_available_index; index > 0; index--) {
                if (std::get<std::vector<TWorld::BulletStateComponent>>(pool.components)[index]
                            .state_machine
                            .is(boost::sml::X)) { pool.remove_at(index); }
            }
        }

        /*
         * Does some reindexing of inactive bullets to make room for new ones
         */
        static void cleanup_bullet_pools(TWorld &world)
        {
            destroy_bullets_outside_frame(world, world.enemy_bullets);
            destroy_bullets_outside_frame(world, world.player_bullets);
            remove_destroyed_bullets(world.enemy_bullets);
            remove_destroyed_bullets(world.player_bullets);
        }
    }// namespace memory

    namespace io
    {
        static void handle_player_input(TWorld &world)
        {
            register_input({KEY_A, KEY_LEFT}, input::PlayerInput::Left, world.player_input);
            register_input({KEY_D, KEY_RIGHT}, input::PlayerInput::Right, world.player_input);
            register_input({KEY_W, KEY_UP}, input::PlayerInput::Up, world.player_input);
            register_input({KEY_S, KEY_DOWN}, input::PlayerInput::Down, world.player_input);
            register_input({KEY_SPACE}, input::PlayerInput::Shoot, world.player_input);
        }
    }// namespace io

    namespace audio
    {
        static void play_sounds(std::vector<assets::SoundId> &sounds_queue, const std::vector<Sound> &sounds)
        {
            for (assets::SoundId &sound : sounds_queue) {
                PlaySound(sounds[static_cast<uint8_t>(sound)]);
            }
            sounds_queue.clear();
        }
    }// namespace audio
}// namespace basilevs

#endif//BASILEVS_CORE_H
