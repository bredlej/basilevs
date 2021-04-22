//
// Created by geoco on 09.04.2021.
//

#ifndef BASILEVS_CORE_H
#define BASILEVS_CORE_H
#include "sprite.h"
#include <chrono>
#include <concepts>
#include <functional>
#include <iostream>
#include <raylib-cpp.hpp>
#include <utility>

namespace config {
    constexpr auto frameWidth = 160;
    constexpr auto frameHeight = 144;
    constexpr auto screenWidth = 800;
    constexpr auto screenHeight = 760;
    namespace colors {
        auto const Foreground = raylib::Color{240, 246, 240};
        auto const Background = raylib::Color{34, 35, 35};
    };
}// namespace config
namespace basilevs {
    enum class EntityType { Player,
                            Enemy,
                            BulletRound,
                            BulletPlayer };


    using TextureMap = std::unordered_map<EntityType, Texture2D>;
    using SpriteTemplateMap = std::unordered_map<EntityType, Sprite>;
    using SoundMap = std::unordered_map<EntityType, Sound>;

}// namespace basilevs

#endif//BASILEVS_CORE_H
