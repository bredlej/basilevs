//
// Created by geoco on 22.06.2021.
//

#ifndef BASILEVS_BACKGROUND_H
#define BASILEVS_BACKGROUND_H
#include <config.h>
namespace behaviours {
    namespace background {

        using UpdateFunction = std::function<void(const double, TWorld &, components::Sprite &, components::Movement &)>;
        constexpr auto level1_background_update = [](const double time, TWorld &, components::Sprite &sprite, components::Movement &) {
          sprite.frame_counter += 1;
          if (sprite.frame_rect.y > 0) {
              if (sprite.frame_counter >= (60 / sprite.frame_speed)) {
                  sprite.frame_counter = 0;
                  sprite.frame_rect.y -= 1 - time;
              }
          }
        };
    }// namespace background
}// namespace behaviours
#endif//BASILEVS_BACKGROUND_H
