//
// Created by geoco on 09.04.2021.
//

#ifndef BASILEVS_CORE_H
#define BASILEVS_CORE_H
#include <raylib-cpp.hpp>
namespace core {
    struct PlayerDefinition {
    public:
        PlayerDefinition(const std::string &animation_file, Vector2 &&position, uint8_t amount_frames)
            : texture{LoadTextureFromImage(raylib::LoadImage(animation_file))},
              position{position},
              current_frame_{0},
              frame_rect{0.0f, 0.0f, static_cast<float>(texture.width) / amount_frames, static_cast<float>(texture.height)},
              amount_frames_{amount_frames} {};
        Vector2 position;
        Texture2D texture;
        Rectangle frame_rect;

        void update_animation(int frame_speed) {
            frame_counter_++;

            if (frame_counter_ >= (60 / frame_speed)) {
                frame_counter_ = 0;
                current_frame_++;

                if (current_frame_ > amount_frames_ - 1) current_frame_ = 0;

                frame_rect.x = (float) current_frame_ * (float) texture.width / amount_frames_;
            }
        }

    private:
        int amount_frames_{1};
        uint8_t current_frame_;
        uint8_t frame_counter_{0};
    };

    struct Background {
    public:
        Background(const std::string &file_name, int vertical_segments)
            : texture{LoadTextureFromImage(raylib::LoadImage(file_name))},
              vertical_segments_{vertical_segments},
              texture_rect{0.0, static_cast<float>(texture.height - (texture.height / vertical_segments)), static_cast<float>(texture.width), static_cast<float>((texture.height / vertical_segments))},
              frame_counter_{0} {};
        Texture2D texture;
        Rectangle texture_rect;

        void update_background(float frame_speed, float frame_time) {
            frame_counter_++;
            if (texture_rect.y > 0) {
                if (frame_counter_ >= (60 / frame_speed)) {
                    frame_counter_ = 0;

                    texture_rect.y -=1 - frame_time;
                }
            }
        }
    private:
        int vertical_segments_;
        int offset;
        int frame_counter_;
    };
}// namespace core

#endif//BASILEVS_CORE_H
