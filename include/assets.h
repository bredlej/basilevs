//
// Created by geoco on 18.05.2021.
//

#ifndef BASILEVS_ASSETS_H
#define BASILEVS_ASSETS_H
#include <raylib.h>
#include <raylib-cpp.hpp>
namespace assets {
    enum class TextureId {
        Player = 0,
        Background_Level_1 = 1,
        Enemy = 2,
        Bullet = 3
    };
    constexpr auto load_textures_level_1 = []() -> std::vector<Texture2D> {
      return {
              LoadTextureFromImage(raylib::LoadImage("assets/player.png")),
              LoadTextureFromImage(raylib::LoadImage("assets/basilevs_bg_001.png")),
              LoadTextureFromImage(raylib::LoadImage("assets/enemy.png")),
              LoadTextureFromImage(raylib::LoadImage("assets/bullet8.png"))};
    };
}


#endif//BASILEVS_ASSETS_H
