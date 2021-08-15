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
#include <raylib-cpp.hpp>
#include <raylib-cpp/include/Functions.hpp>
#include <raylib-cpp/include/Vector2.hpp>
#include <raylib.h>
#include <utility>
#include <world.h>

namespace basilevs {
    enum class EntityType { Player };

    // TODO replace with common function for both player and enemies
    static constexpr auto setup_player_sprite = [](components::Sprite &sprite_component, const std::vector<Texture2D> &textures, const auto texture, const auto amount_frames) {
        sprite_component.current_frame = std::rand() % amount_frames;
        sprite_component.texture = texture;
        sprite_component.amount_frames = amount_frames;
        auto texture_width = textures[static_cast<int>(sprite_component.texture)].width;
        auto texture_height = textures[static_cast<int>(sprite_component.texture)].height;
        sprite_component.texture_width = texture_width;
        sprite_component.texture_height = texture_height;
        sprite_component.frame_speed = 12;
        sprite_component.frame_rect = {0.0f, 0.0f, static_cast<float>(texture_width) / static_cast<float>(sprite_component.amount_frames), static_cast<float>(texture_height)};
    };

    static constexpr auto setup_sprite_component = [](components::Sprite &sprite_component, const std::vector<Texture2D> &textures, const behaviours::enemy::EnemyDefinition enemy_definition) {
        sprite_component.current_frame = std::rand() % enemy_definition.amount_frames;
        sprite_component.texture = enemy_definition.texture;
        sprite_component.amount_frames = enemy_definition.amount_frames;
        auto texture_width = textures[static_cast<int>(sprite_component.texture)].width;
        auto texture_height = textures[static_cast<int>(sprite_component.texture)].height;
        sprite_component.texture_width = texture_width;
        sprite_component.texture_height = texture_height;
        sprite_component.frame_speed = 12;
        sprite_component.frame_rect = {0.0f, 0.0f, static_cast<float>(texture_width) / static_cast<float>(sprite_component.amount_frames), static_cast<float>(texture_height)};
    };

    static constexpr auto make_player_blueprint = [](const std::vector<Texture2D> &textures) {
        auto player = Blueprint(behaviours::player::UpdateFunction(behaviours::player::kPlayerNormalBehaviour));
        setup_player_sprite(get<components::Sprite>(player), textures, assets::TextureId::Player, 7);
        auto &movement_component = get<components::Movement>(player);
        movement_component.position = raylib::Vector2{70, 100};
        movement_component.speed = 50.0;
        return std::make_shared<decltype(player)>(player);
    };

    static constexpr auto make_background_blueprint = [](const std::vector<Texture2D> &textures) {
        auto background = Blueprint(behaviours::background::UpdateFunction(behaviours::background::level1_background_update));
        auto &background_sprite = get<components::Sprite>(background);
        auto texture = textures[static_cast<int>(assets::TextureId::Background_Level_1)];
        background_sprite.texture = assets::TextureId::Background_Level_1;
        background_sprite.amount_frames = 6;
        background_sprite.texture_width = texture.width;
        background_sprite.texture_height = texture.height;
        background_sprite.frame_speed = 12;
        background_sprite.frame_rect = {0.0, static_cast<float>(texture.height) - (static_cast<float>(texture.height) / static_cast<float>(background_sprite.amount_frames)), static_cast<float>(texture.width), static_cast<float>(texture.height) / static_cast<float>(background_sprite.amount_frames)};

        return std::make_shared<decltype(background)>(background);
    };

    static constexpr auto spawn_enemy_after_seconds = [](const double time, const std::vector<Texture2D> &textures, const Vector2 &&position, const behaviours::enemy::EnemyDefinition &enemy_definition) {
        auto enemy = Blueprint(behaviours::enemy::UpdateFunction(enemy_definition.behaviour));
        setup_sprite_component(get<components::Sprite>(enemy), textures, enemy_definition);
        get<components::Movement>(enemy).position = position;
        get<components::TimeCounter>(enemy).elapsed_time = 0.0;
        get<components::Emission>(enemy).last_emission = 0.0f;
        auto &activation_component = get<components::Activation>(enemy);
        activation_component.activate_after_time = time;
        activation_component.is_active = false;
        return enemy;
    };

    static constexpr auto initialize_enemies = [](const std::vector<Texture2D> &textures) {
        auto enemies_in_memory = BlueprintsInMemory(
                spawn_enemy_after_seconds(1.1, textures, {0, 20}, behaviours::enemy::tentacle()),
                spawn_enemy_after_seconds(2.2, textures, {35, 50}, behaviours::enemy::mosquito()),
                spawn_enemy_after_seconds(3.3, textures, {70, 40},  behaviours::enemy::tentacle()),
                spawn_enemy_after_seconds(4.4, textures, {105, 50}, behaviours::enemy::mosquito()),
                spawn_enemy_after_seconds(5.5, textures, {130, 30}, behaviours::enemy::tentacle()));
        return std::make_shared<decltype(enemies_in_memory)>(enemies_in_memory);
    };

    static constexpr auto render_to_texture = [](raylib::RenderTexture &render_target, const TWorld &world, const std::vector<Texture2D> &textures) {
        static constexpr auto render_player_sprite = [](raylib::RenderTexture &render_target, const TWorld &world, const std::vector<Texture2D> &textures) {
            auto player = world.player.get();
            auto sprite_component = std::get<components::Sprite>(player->components);
            auto movement_component = std::get<components::Movement>(player->components);

            auto texture = textures[static_cast<int>(sprite_component.texture)];

            DrawTextureRec(texture, sprite_component.frame_rect, movement_component.position, WHITE);
            //DrawCircleLines(static_cast<int>(movement_component.position.x + 16), static_cast<int>(movement_component.position.y + 16), 4.0f, RED);
        };

        static constexpr auto render_enemy_sprites = [](raylib::RenderTexture &render_target, const TWorld &world, const std::vector<Texture2D> &textures) {
            auto enemy_components = world.enemies->components;
            auto sprites = std::get<std::vector<components::Sprite>>(enemy_components);
            auto activations = std::get<std::vector<components::Activation>>(enemy_components);
            auto movement = std::get<std::vector<components::Movement>>(enemy_components);

            for (size_t i = 0; i < sprites.size(); i++) {
                if (activations[i].is_active) {
                    DrawTextureRec(textures[static_cast<int>(sprites[i].texture)], sprites[i].frame_rect, movement[i].position, WHITE);
                }
            }
        };

        static constexpr auto render_bullet_sprites = [](raylib::RenderTexture &render_target, const TWorld &world, const std::vector<Texture2D> &textures) {
            auto enemy_components = world.enemy_bullets.components;
            auto enemy_bullet_sprites = std::get<std::vector<components::Sprite>>(enemy_components);
            auto enemy_bullet_movements = std::get<std::vector<components::Movement>>(enemy_components);

            auto player_components = world.player_bullets.components;
            auto player_bullet_sprites = std::get<std::vector<components::Sprite>>(player_components);
            auto player_bullet_movements = std::get<std::vector<components::Movement>>(player_components);

            for (std::size_t i = 0; i < world.enemy_bullets.first_available_index; i++) {
                const auto bullet_position = enemy_bullet_movements[i].position;
                //DrawTextureEx(textures[static_cast<int>(enemy_bullet_sprites[i].texture)], bullet_position, enemy_bullet_sprites[i].rotation, 1.0f, WHITE);
                DrawTextureRec(textures[static_cast<int>(enemy_bullet_sprites[i].texture)], enemy_bullet_sprites[i].frame_rect, bullet_position, WHITE);
            }
            for (std::size_t i = 0; i < world.player_bullets.first_available_index; i++) {
                const auto bullet_position = player_bullet_movements[i].position;
                DrawTextureEx(textures[static_cast<int>(player_bullet_sprites[i].texture)], bullet_position, player_bullet_sprites[i].rotation, 1.0f, WHITE);
            }
        };

        static constexpr auto render_background = [](raylib::RenderTexture &render_target, const TWorld &world, const std::vector<Texture2D> &textures) {
            auto background = world.background.get();
            auto sprite_component = std::get<components::Sprite>(background->components);
            auto texture = textures[static_cast<int>(sprite_component.texture)];

            DrawTextureRec(texture, sprite_component.frame_rect, {0, 0}, WHITE);
        };

        BeginTextureMode(render_target);
        ClearBackground(config::colors::kBackground);
        render_background(render_target, world, textures);
        render_player_sprite(render_target, world, textures);
        render_enemy_sprites(render_target, world, textures);
        render_bullet_sprites(render_target, world, textures);
        EndTextureMode();
    };

    static constexpr auto render_to_screen = [](raylib::RenderTexture &render_target, const TWorld &world) {
        DrawTexturePro(render_target.texture, Rectangle{0.0f, 0.0f, (float) render_target.texture.width, (float) -render_target.texture.height},
                       raylib::Rectangle{0.0f, 0.0f, static_cast<float>(config::kScreenWidth), static_cast<float>(config::kScreenHeight)}, Vector2{0, 0}, 0.0f, WHITE);
        raylib::DrawText(std::to_string(world.enemy_bullets.first_available_index), config::kScreenWidth - 60, 60, 30, ORANGE);
        raylib::DrawText(std::to_string(world.player_bullets.first_available_index), config::kScreenWidth - 60, 95, 30, ORANGE);

        DrawFPS(5, 5);
    };

    static constexpr auto cleanup_bullet_pools = [](auto &world) {
        static constexpr auto destroy_bullets_outside_frame = [](const auto world, auto &pool) {
            for (std::size_t index = pool.first_available_index; index > 0; index--) {
                if (!raylib::Rectangle(std::get<std::vector<components::Movement>>(pool.components)[index - 1].position,
                                       std::get<std::vector<components::Sprite>>(pool.components)[index - 1].bounds)
                                       .CheckCollision(world.frame_bounds)) {
                    std::get<std::vector<TWorld::BulletStateComponent>>(pool.components)[index - 1]
                    .state_machine
                    .process_event(state::DestroyEvent());
                }
            }
        };
        static constexpr auto remove_destroyed_pool_objects = [&](auto &pool) {
            for (std::size_t index = pool.first_available_index; index > 0; index--) {
                if (std::get<std::vector<TWorld::BulletStateComponent>>(pool.components)[index - 1]
                .state_machine
                .is(boost::sml::X)) { pool.remove_at(index - 1); }
            }
        };
        destroy_bullets_outside_frame(world, world.enemy_bullets);
        destroy_bullets_outside_frame(world, world.player_bullets);
        remove_destroyed_pool_objects(world.enemy_bullets);
        remove_destroyed_pool_objects(world.player_bullets);
    };

    static constexpr auto handle_player_input = [](TWorld &world) {
        static constexpr auto register_input = [](const std::initializer_list<int32_t> keys, const auto action, auto &world) {
            bool is_key_down = false;
            bool is_key_up = false;
            for (auto key: keys) {
                is_key_down |= IsKeyDown(key);
                is_key_up |= IsKeyUp(key);
            }
            if (is_key_down) {
                world.player_input.set(action);
            }
            else if (is_key_up) {
                world.player_input.set(action, false);
            }
        };
        register_input({KEY_A, KEY_LEFT},input::PlayerInput::Left, world);
        register_input({KEY_D, KEY_RIGHT},input::PlayerInput::Right,world);
        register_input({KEY_W, KEY_UP}, input::PlayerInput::Up, world);
        register_input({KEY_S, KEY_DOWN}, input::PlayerInput::Down, world);
        register_input({KEY_SPACE}, input::PlayerInput::Shoot, world);
    };

    static constexpr auto play_sounds = [](std::vector<assets::SoundId> &sounds_queue, const std::vector<Sound> &sounds) {
        for (auto &sound : sounds_queue) {
            PlaySound(sounds[static_cast<uint8_t>(sound)]);
        }
        sounds_queue.clear();
    };
}// namespace basilevs

#endif//BASILEVS_CORE_H
