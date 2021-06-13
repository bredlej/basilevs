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
#include "sprite.h"
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

    using TextureMap = std::unordered_map<EntityType, Texture2D>;
    using SpriteTemplateMap = std::unordered_map<EntityType, Sprite>;
    using SoundMap = std::unordered_map<EntityType, Sound>;

    constexpr auto initialize_player = [](const std::vector<Texture2D> &textures) {
      auto player = Blueprint(behaviours::player::UpdateFunction(behaviours::player::kPlayerNormalBehaviour));
      auto &player_sprite = get<components::Sprite>(player);
      player_sprite.texture = assets::TextureId::Player;
      player_sprite.amount_frames = 7;
      auto texture_width = textures[static_cast<int>(player_sprite.texture)].width;
      auto texture_height = textures[static_cast<int>(player_sprite.texture)].height;
      player_sprite.texture_width = texture_width;
      player_sprite.texture_height = texture_height;
      player_sprite.frame_rect = {0.0f, 0.0f, static_cast<float>(texture_width) / static_cast<float>(player_sprite.amount_frames), static_cast<float>(texture_height)};

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
      background_sprite.frame_rect = {0.0, static_cast<float>(texture.height - (texture.height / background_sprite.amount_frames)), static_cast<float>(texture.width), static_cast<float>((texture.height / background_sprite.amount_frames))};

      return std::make_shared<decltype(background)>(background);
    };

    constexpr auto render_player = [](raylib::RenderTexture &render_target, const TWorld &world, const std::vector<Texture2D> &textures) {
      auto player = world.player.get();
      auto sprite_component = std::get<components::Sprite>(player->components);
      auto texture = textures[static_cast<int>(sprite_component.texture)];

      DrawTextureRec(texture, sprite_component.frame_rect, {20, 20}, WHITE);
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
