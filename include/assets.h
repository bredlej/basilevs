//
// Created by geoco on 18.05.2021.
//

#ifndef BASILEVS_ASSETS_H
#define BASILEVS_ASSETS_H
#include <raylib-cpp.hpp>
#include <raylib.h>
namespace assets {
    enum class TextureId {
        Player = 0,
        Background_Level_1 = 1,
        Enemy = 2,
        Bullet = 3
    };
    enum class SoundId : uint8_t {
        NormalBullet
    };

    constexpr auto texture_of_image = [](auto &&image) {
        auto texture = LoadTextureFromImage(image);
        UnloadImage(image);
        return texture;
    };
    constexpr auto load_textures_level_1 = []() -> std::vector<Texture2D> {
        return {
                texture_of_image(raylib::LoadImage("assets/player.png")),
                texture_of_image(raylib::LoadImage("assets/basilevs_bg_001.png")),
                texture_of_image(raylib::LoadImage("assets/enemy.png")),
                texture_of_image(raylib::LoadImage("assets/bullet8.png"))};
    };

    constexpr auto load_sounds = []() -> std::vector<Sound> {
        return {
                LoadSound("assets/bullet.wav")};
    };
}// namespace assets


#endif//BASILEVS_ASSETS_H
