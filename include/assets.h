//
// Created by geoco on 18.05.2021.
//

#ifndef BASILEVS_ASSETS_H
#define BASILEVS_ASSETS_H
#include <core.hpp>
#include <raylib.h>
#include <unordered_map>

namespace assets
{
    using namespace entt::literals;

    enum class TextureId
    {
        Player = 0,
        Background_Level_1 = 1,
        Enemy = 2,
        Bullet_Tentacle = 3,
        Player_Bullet = 4,
        Tentacle = 5,
        Mosquito = 6,
        Bullet_Mosquito = 7
    };

    inline std::unordered_map<TextureId, entt::hashed_string> texture_id_to_string = {
            {TextureId::Player, "assets/player.png"_hs},
            {TextureId::Background_Level_1, "assets/basilevs_bg_001.png"_hs},
            {TextureId::Enemy, "assets/enemy.png"_hs},
            {TextureId::Bullet_Tentacle, "assets/bullet8.png"_hs},
            {TextureId::Player_Bullet, "assets/bullet8-002.png"_hs},
            {TextureId::Tentacle, "assets/tentacle-0002.png"_hs},
            {TextureId::Mosquito, "assets/mosquito-0001.png"_hs},
            {TextureId::Bullet_Mosquito, "assets/bullet8-003.png"_hs}};

    enum class SoundId
    {
        NormalBullet
    };

    inline std::unordered_map<SoundId, entt::hashed_string> sound_id_to_string = {
            {SoundId::NormalBullet, "assets/bullet.wav"_hs}}
    ;

    constexpr auto texture_of_image = [](auto &&image)
    {
        auto texture = LoadTextureFromImage(image);
        UnloadImage(image);
        return texture;
    };

    inline void load_texture_cache(entt::resource_cache<Texture, Texture2DLoader> &cache)
    {
        using namespace entt::literals;
        cache.load(texture_id_to_string[TextureId::Player], "assets/player.png");
        cache.load(texture_id_to_string[TextureId::Background_Level_1], "assets/basilevs_bg_001.png");
        cache.load(texture_id_to_string[TextureId::Enemy], "assets/enemy.png");
        cache.load(texture_id_to_string[TextureId::Bullet_Tentacle], "assets/bullet8.png");
        cache.load(texture_id_to_string[TextureId::Player_Bullet], "assets/bullet8-002.png");
        cache.load(texture_id_to_string[TextureId::Tentacle], "assets/tentacle-0002.png");
        cache.load(texture_id_to_string[TextureId::Mosquito], "assets/mosquito-0001.png");
        cache.load(texture_id_to_string[TextureId::Bullet_Mosquito], "assets/bullet8-003.png");
    }

    inline void load_sound_cache(entt::resource_cache<Sound, SoundLoader> &cache)
    {
        using namespace entt::literals;
        cache.load(sound_id_to_string[SoundId::NormalBullet], "assets/bullet.wav");
    }
}// namespace assets


#endif//BASILEVS_ASSETS_H
