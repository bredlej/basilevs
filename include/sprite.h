//
// Created by geoco on 22.04.2021.
//

#ifndef BASILEVS_SPRITE_H
#define BASILEVS_SPRITE_H
#include <cstdint>
#include <raylib.h>

struct Sprite {
public:
    Sprite() = default;
    Sprite(const Texture2D &texture, Vector2 &&position, const uint8_t amount_frames)
            : texture{texture},
              position{position},
              current_frame_{0},
              frame_rect{0.0f, 0.0f, static_cast<float>(texture.width) / static_cast<float>(amount_frames), static_cast<float>(texture.height)},
              amount_frames_{amount_frames} {};

    Vector2 position{0.0f, 0.0f};
    Texture2D texture{Texture2D{}};
    Rectangle frame_rect {0.0f, 0.0f, 0.0f, 0.0f};

    float inner_timer{0.0f};

    void update_animation(int frame_speed);
private:
    int amount_frames_{1};
    uint8_t current_frame_{0};
    uint8_t frame_counter_{0};
};
#endif//BASILEVS_SPRITE_H
