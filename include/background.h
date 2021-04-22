//
// Created by geoco on 22.04.2021.
//

#ifndef BASILEVS_BACKGROUND_H
#define BASILEVS_BACKGROUND_H
#include <raylib.h>
#include <raylib-cpp.hpp>
struct Background {
public:
    Background() = default;
    Background(const std::string &file_name, const int vertical_segments)
            : texture{LoadTextureFromImage(raylib::LoadImage(file_name))},
              amount_frames{vertical_segments},
              texture_rect{0.0, static_cast<float>(texture.height - (texture.height / vertical_segments)), static_cast<float>(texture.width), static_cast<float>((texture.height / vertical_segments))},
              frame_counter_{0} {};
    Texture2D texture;
    Rectangle texture_rect;

    void update_background(const float frame_speed, const float frame_time) {
        frame_counter_ += 1;
        if (texture_rect.y > 0) {
            if (frame_counter_ >= (60 / frame_speed)) {
                frame_counter_ = 0;
                texture_rect.y -= 1 - frame_time;
            }
        }
    }

private:
    int amount_frames;
    int frame_counter_;
};
#endif//BASILEVS_BACKGROUND_H
