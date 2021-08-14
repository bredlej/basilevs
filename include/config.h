//
// Created by geoco on 22.06.2021.
//

#ifndef BASILEVS_CONFIG_H
#define BASILEVS_CONFIG_H
namespace config {
    constexpr auto kFrameWidth = 160;
    constexpr auto kFrameHeight = 144;
    constexpr auto kFrameBoundLeft = -32;
    constexpr auto kFrameBoundUp = -32;
    constexpr auto kFrameBoundRight = 180;
    constexpr auto kFrameBoundDown = 180;
    constexpr auto kScreenWidth = 800;
    constexpr auto kScreenHeight = 720;
    constexpr auto kEnemyBulletPoolSize = 10;
    constexpr auto kPlayerBulletPoolSize = 10;
    constexpr auto kSoundQueueSize = 16;
    namespace colors {
        auto const kForeground = raylib::Color{240, 246, 240};
        auto const kBackground = raylib::Color{34, 35, 35};
    };// namespace colors
}// namespace config
#endif//BASILEVS_CONFIG_H
