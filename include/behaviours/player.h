//
// Created by geoco on 22.06.2021.
//

#ifndef BASILEVS_PLAYER_H
#define BASILEVS_PLAYER_H
#include <world.h>
#include <config.h>
namespace behaviours {
    namespace player {
        constexpr auto frame_update = [](components::Sprite &sprite) {
            sprite.frame_counter++;

            if (sprite.frame_counter >= (60 / sprite.frame_speed)) {
                sprite.frame_counter = 0;
                sprite.current_frame++;

                if (sprite.current_frame > sprite.amount_frames - 1) sprite.current_frame = 0;

                sprite.frame_rect.x = static_cast<float>(sprite.current_frame) * static_cast<float>(sprite.texture_width) / static_cast<float>(sprite.amount_frames);
            }
        };
        constexpr auto move = [] (const double time, TWorld &world, components::Movement &movement) {
            if (world.player_input[input::PlayerInput::Left]) {
                movement.position.x -= movement.speed * time;
            }
            if (world.player_input[input::PlayerInput::Right]) {
                movement.position.x += movement.speed * time;
            }
            if (world.player_input[input::PlayerInput::Up]) {
                movement.position.y -= movement.speed * time;
            }
            if (world.player_input[input::PlayerInput::Down]) {
                movement.position.y += movement.speed * time;
            }
        };
        constexpr auto kPlayerNormalBehaviour = [](const double time, TWorld &world, components::Sprite &sprite, components::Movement &movement) {
            frame_update(sprite);
            move(time, world, movement);
        };
        using UpdateFunction = std::function<void(const double, TWorld &, components::Sprite &, components::Movement &)>;
    }// namespace player
}
#endif//BASILEVS_PLAYER_H
