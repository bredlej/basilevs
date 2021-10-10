//
// Created by geoco on 22.06.2021.
//

#ifndef BASILEVS_CONFIG_H
#define BASILEVS_CONFIG_H
namespace config
{
    constexpr auto kFrameWidth = 160;
    constexpr auto kFrameHeight = 144;
    constexpr auto kFrameBoundLeft = -64;
    constexpr auto kFrameBoundUp = -64;
    constexpr auto kFrameBoundRight = 200;
    constexpr auto kFrameBoundDown = 200;
    constexpr auto kScreenWidth = 960;
    constexpr auto kScreenHeight = 864;
    constexpr auto kEnemyBulletPoolSize = 200;
    constexpr auto kPlayerBulletPoolSize = 100;
    constexpr auto kSoundQueueSize = 16;
    namespace colors
    {
        auto const kForeground = {240, 246, 240};
        auto const kBackground = std::vector{34, 35, 35};
    };// namespace colors
}// namespace config
#endif//BASILEVS_CONFIG_H
