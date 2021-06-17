//
// Created by geoco on 09.04.2021.
//

#ifndef BASILEVS_CORE_H
#define BASILEVS_CORE_H
#include <raylib-cpp/include/Functions.hpp>
#include <raylib-cpp/include/Vector2.hpp>
#include <raylib.h>
#include <world.h>
#include <behaviours.h>
#include <chrono>
#include <concepts>
#include <functional>
#include <iostream>
#include <raylib-cpp.hpp>
#include <utility>

namespace config {
    constexpr auto kFrameWidth = 160;
    constexpr auto kFrameHeight = 144;
    constexpr auto kScreenWidth = 800;
    constexpr auto kScreenHeight = 760;
    namespace colors {
        auto const kForeground = raylib::Color{240, 246, 240};
        auto const kBackground = raylib::Color{34, 35, 35};
    };// namespace colors
}// namespace config
namespace basilevs {
    enum class EntityType { Player,
                            Enemy,
                            BulletRound,
                            BulletPlayer };

    constexpr auto prepare_sprite = [](components::Sprite &sprite_component, const std::vector<Texture2D> &textures, const assets::TextureId texture_id, const uint32_t amount_frames) {
      sprite_component.texture = texture_id;
      sprite_component.amount_frames = amount_frames;
      auto texture_width = textures[static_cast<int>(sprite_component.texture)].width;
      auto texture_height = textures[static_cast<int>(sprite_component.texture)].height;
      sprite_component.texture_width = texture_width;
      sprite_component.texture_height = texture_height;
      sprite_component.frame_rect = {0.0f, 0.0f, static_cast<float>(texture_width) / static_cast<float>(sprite_component.amount_frames), static_cast<float>(texture_height)};
    };

    constexpr auto initialize_player = [](const std::vector<Texture2D> &textures) {
      auto player = Blueprint(behaviours::player::UpdateFunction(behaviours::player::kPlayerNormalBehaviour));
      auto &player_sprite = get<components::Sprite>(player);
      prepare_sprite(player_sprite, textures, assets::TextureId::Player, 7);
      auto &movement_component = get<components::Movement>(player);
      movement_component.position = {70, 100};
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
      return enemy;
    };

    constexpr auto initialize_enemies = [](const std::vector<Texture2D> &textures) {
        auto enemies_in_memory = BlueprintsInMemory(
                spawn_enemy_after_seconds(1, textures, {0, 20}),
                spawn_enemy_after_seconds(2, textures, {35, 20}),
                spawn_enemy_after_seconds(3, textures, {70, 20}),
                spawn_enemy_after_seconds(4, textures, {105, 20}),
                spawn_enemy_after_seconds(5, textures, {140, 20}));
        return std::make_shared<decltype(enemies_in_memory)>(enemies_in_memory);
    };

    constexpr auto render_player = [](raylib::RenderTexture &render_target, const TWorld &world, const std::vector<Texture2D> &textures) {
      auto player = world.player.get();
      auto sprite_component = std::get<components::Sprite>(player->components);
      auto movement_component = std::get<components::Movement>(player->components);

      auto texture = textures[static_cast<int>(sprite_component.texture)];

      DrawTextureRec(texture, sprite_component.frame_rect, movement_component.position, WHITE);
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
      EndTextureMode();
    };

    constexpr auto render_to_screen = [] (raylib::RenderTexture & render_target, const TWorld &world) {
      DrawTexturePro(render_target.texture, Rectangle{0.0f, 0.0f, (float) render_target.texture.width, (float) -render_target.texture.height},
                     Rectangle{0.0f, 0.0f, static_cast<float>(config::kScreenWidth), static_cast<float>(config::kScreenHeight)}, Vector2{0, 0}, 0.0f, WHITE);
      //raylib::DrawText(std::to_string(static_cast<int>(world.timer)), config::kScreenWidth - 60, 20, 30, GREEN);
      raylib::DrawText(std::to_string(world.enemy_bullets.first_available_index), config::kScreenWidth - 60, 60, 30, ORANGE);
      DrawFPS(5, 5);
    };


}// namespace basilevs

#endif//BASILEVS_CORE_H
