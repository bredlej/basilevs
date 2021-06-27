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

    constexpr auto prepare_sprite = [](components::Sprite &sprite_component, const std::vector<Texture2D> &textures, const assets::TextureId texture_id, const uint32_t amount_frames) {
        sprite_component.texture = texture_id;
        sprite_component.amount_frames = amount_frames;
        auto texture_width = textures[static_cast<int>(sprite_component.texture)].width;
        auto texture_height = textures[static_cast<int>(sprite_component.texture)].height;
        sprite_component.texture_width = texture_width;
        sprite_component.texture_height = texture_height;
        sprite_component.frame_speed = 12;
        sprite_component.frame_rect = {0.0f, 0.0f, static_cast<float>(texture_width) / static_cast<float>(sprite_component.amount_frames), static_cast<float>(texture_height)};
    };

    constexpr auto initialize_player = [](const std::vector<Texture2D> &textures) {
        auto player = Blueprint(behaviours::player::UpdateFunction(behaviours::player::kPlayerNormalBehaviour));
        auto &player_sprite = get<components::Sprite>(player);
        prepare_sprite(player_sprite, textures, assets::TextureId::Player, 7);
        auto &movement_component = get<components::Movement>(player);
        movement_component.position = raylib::Vector2{70, 100};
        movement_component.speed = 50.0;
        return std::make_shared<decltype(player)>(player);
    };

    constexpr auto initialize_background = [](const std::vector<Texture2D> &textures) {
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

    constexpr auto spawn_enemy_after_seconds = [](const double time, const std::vector<Texture2D> &textures, const Vector2 &&position) {
        auto enemy = Blueprint(behaviours::enemy::UpdateFunction(behaviours::enemy::kEnemyNormal));
        auto &activation_component = get<components::Activation>(enemy);
        activation_component.activate_after_time = time;
        activation_component.is_active = false;
        auto &sprite_component = get<components::Sprite>(enemy);
        prepare_sprite(sprite_component, textures, assets::TextureId::Enemy, 1);
        auto &movement_component = get<components::Movement>(enemy);
        movement_component.position = position;
        auto &time_counter = get<components::TimeCounter>(enemy);
        time_counter.elapsed_time = 0.0;
        auto &emitter = get<components::Emission>(enemy);
        emitter.last_emission = 0.0f;
        return enemy;
    };

    constexpr auto initialize_enemies = [](const std::vector<Texture2D> &textures) {
        auto enemies_in_memory = BlueprintsInMemory(
                spawn_enemy_after_seconds(1.1, textures, {0, 20}),
                spawn_enemy_after_seconds(2.2, textures, {35, 20}),
                spawn_enemy_after_seconds(3.3, textures, {70, 20}),
                spawn_enemy_after_seconds(4.4, textures, {105, 20}),
                spawn_enemy_after_seconds(5.5, textures, {140, 20}));
        return std::make_shared<decltype(enemies_in_memory)>(enemies_in_memory);
    };

    constexpr auto render_player = [](raylib::RenderTexture &render_target, const TWorld &world, const std::vector<Texture2D> &textures) {
        auto player = world.player.get();
        auto sprite_component = std::get<components::Sprite>(player->components);
        auto movement_component = std::get<components::Movement>(player->components);

        auto texture = textures[static_cast<int>(sprite_component.texture)];

        DrawTextureRec(texture, sprite_component.frame_rect, movement_component.position, WHITE);
        DrawCircleLines(movement_component.position.x + 16, movement_component.position.y + 16, 4.0f, RED);
    };

    constexpr auto render_enemies = [](raylib::RenderTexture &render_target, const TWorld &world, const std::vector<Texture2D> &textures) {
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

    constexpr auto render_bullets = [](raylib::RenderTexture &render_target, const TWorld &world, const std::vector<Texture2D> &textures) {
        auto enemy_components = world.enemy_bullets.components;
        auto sprites = std::get<std::vector<components::Sprite>>(enemy_components);
        auto movement = std::get<std::vector<components::Movement>>(enemy_components);

        for (std::size_t i = 0; i < world.enemy_bullets.first_available_index; i++) {
            const auto bullet_position = movement[i].position;
            //DrawTextureRec(textures[static_cast<int>(sprites[i].texture)], sprites[i].frame_rect, bullet_position, WHITE);
            DrawTextureEx(textures[static_cast<int>(sprites[i].texture)], bullet_position, sprites[i].rotation, 1.0f, WHITE);
            //  raylib::DrawText(std::to_string(i), bullet_position.x - 5, bullet_position.y -5, 7, ORANGE);
        }
    };

    constexpr auto render_background = [](raylib::RenderTexture &render_target, const TWorld &world, const std::vector<Texture2D> &textures) {
        auto background = world.background.get();
        auto sprite_component = std::get<components::Sprite>(background->components);
        auto texture = textures[static_cast<int>(sprite_component.texture)];

        DrawTextureRec(texture, sprite_component.frame_rect, {0, 0}, WHITE);
    };

    constexpr auto render_to_texture = [](raylib::RenderTexture &render_target, const TWorld &world, const std::vector<Texture2D> &textures) {
        BeginTextureMode(render_target);
        ClearBackground(config::colors::kBackground);
        render_background(render_target, world, textures);
        render_player(render_target, world, textures);
        render_enemies(render_target, world, textures);
        render_bullets(render_target, world, textures);
        EndTextureMode();
    };

    constexpr auto render_to_screen = [](raylib::RenderTexture &render_target, const TWorld &world) {
        DrawTexturePro(render_target.texture, Rectangle{0.0f, 0.0f, (float) render_target.texture.width, (float) -render_target.texture.height},
                       raylib::Rectangle{0.0f, 0.0f, static_cast<float>(config::kScreenWidth), static_cast<float>(config::kScreenHeight)}, Vector2{0, 0}, 0.0f, WHITE);
        //raylib::DrawText(std::to_string(static_cast<int>(world.timer)), config::kScreenWidth - 60, 20, 30, GREEN);
        raylib::DrawText(std::to_string(world.enemy_bullets.first_available_index), config::kScreenWidth - 60, 60, 30, ORANGE);
        if (IsKeyDown(KEY_A)) {
            raylib::DrawText("Left", config::kScreenWidth - 60, 90, 30, ORANGE);
        }
        DrawFPS(5, 5);
    };

    constexpr auto bullets_bounds_check = [](TWorld &world) {
        const auto movements = std::get<std::vector<components::Movement>>(world.enemy_bullets.components);
        const auto bullet_size = raylib::Vector2(8, 8);

        for (std::size_t index = world.enemy_bullets.first_available_index; index > 0; index--) {
            const auto bullet_movement = movements[index];
            const auto bullet_bounds = raylib::Rectangle(bullet_movement.position, bullet_size);
            if (!bullet_bounds.CheckCollision(world.bounds)) {
                world.enemy_bullets.remove_at(index);
            }
        }
    };

    constexpr auto handle_player_input = [](TWorld &world) {
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            world.player_input.set(input::PlayerInput::Left);
        } else if (IsKeyUp(KEY_A) || IsKeyUp(KEY_LEFT)) {
            world.player_input.set(input::PlayerInput::Left, false);
        }
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            world.player_input.set(input::PlayerInput::Right);
        } else if (IsKeyUp(KEY_D) || IsKeyUp(KEY_RIGHT)) {
            world.player_input.set(input::PlayerInput::Right, false);
        }
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
            world.player_input.set(input::PlayerInput::Up);
        } else if (IsKeyUp(KEY_W) || IsKeyUp(KEY_UP)) {
            world.player_input.set(input::PlayerInput::Up, false);
        }
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
            world.player_input.set(input::PlayerInput::Down);
        } else if (IsKeyUp(KEY_S) || IsKeyUp(KEY_DOWN)) {
            world.player_input.set(input::PlayerInput::Down, false);
        }
    };

    constexpr auto play_sounds = [](std::vector<assets::SoundId> &sounds_queue, const std::vector<Sound> &sounds) {
        for (auto &sound : sounds_queue) {
            PlaySound(sounds[static_cast<uint8_t>(sound)]);
        }
        sounds_queue.clear();
    };
}// namespace basilevs

#endif//BASILEVS_CORE_H
