//
// Created by Patryk Szczypień on 08/11/2024.
//

#ifndef INCREMENTAL_CLICKER_CORE_HPP
#define INCREMENTAL_CLICKER_CORE_HPP
#include <entt/entt.hpp>
#include <entt/resource/cache.hpp>
#include <pcg/pcg_random.hpp>
extern "C" {
#include <raylib.h>
}

struct Texture2DLoader final {
    using result_type = std::shared_ptr<Texture2D>;

    result_type operator()(const std::string &filename) const {
        return std::make_shared<Texture2D>(LoadTexture(filename.c_str()));
    }
};

struct SoundLoader final {
    using result_type = std::shared_ptr<Sound>;

    result_type operator()(const std::string &filename) const {
        return std::make_shared<Sound>(LoadSound(filename.c_str()));
    }
};

class Core {
public:
    using TextureCache = entt::resource_cache<Texture2D, Texture2DLoader>;
    using SoundCache = entt::resource_cache<Sound, SoundLoader>;
    entt::scheduler scheduler;
    entt::registry registry;
    entt::dispatcher dispatcher;
    TextureCache texture2d_cache{};
    SoundCache sound_cache{};
    pcg32 pcg;
};
#endif //INCREMENTAL_CLICKER_CORE_HPP
