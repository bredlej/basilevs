//
// Created by geoco on 22.04.2021.
//
#include <sprite.h>
void Sprite::update_animation(int frame_speed) {
    frame_counter_++;

    if (frame_counter_ >= (60 / frame_speed)) {
        frame_counter_ = 0;
        current_frame_++;

        if (current_frame_ > amount_frames_ - 1) current_frame_ = 0;

        frame_rect.x = (float) current_frame_ * (float) texture.width / amount_frames_;
    }
}
