//
// Created by geoco on 22.06.2021.
//

#ifndef BASILEVS_BACKGROUND_H
#define BASILEVS_BACKGROUND_H
#include <components.h>
#include <config.h>
#include <functional>
class TWorld;
namespace behaviours
{
    namespace background
    {

        /*
         * Declares components used when updating the background.
         * Components must be the same as declared in the alias
         */
        using UpdateFunction =
                std::function<void(
                        const double,
                        TWorld &,
                        components::Sprite &,
                        components::Movement &)>;
        /*
         * Describes how to animate the background in Level 1.
         * This is a simple upwards scroll of the visible frame to give the illusion that the player is flying up in the skies
         */
        constexpr auto level1_background_update = [](const double time, TWorld &, components::Sprite &sprite, components::Movement &)
        {
            sprite.fps_counter += 1;
            if (sprite.frame_rect.y > 0) {
                if (sprite.fps_counter >= (60 / sprite.fps_speed)) {
                    sprite.fps_counter = 0;
                    sprite.frame_rect.y -= 1 - time;
                }
            }
        };
    }// namespace background
}// namespace behaviours
#endif//BASILEVS_BACKGROUND_H
