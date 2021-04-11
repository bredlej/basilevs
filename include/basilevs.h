//
// Created by geoco on 09.04.2021.
//

#ifndef BASILEVS_CORE_H
#define BASILEVS_CORE_H
#include <concepts>
#include <functional>
#include <raylib-cpp.hpp>
#include <utility>
namespace basilevs {
    struct Sprite;
    struct Background;
    struct Enemy;
    struct BulletBase;
    struct NormalBullet;
    template<typename T>
    concept is_a_bullet = std::is_base_of_v<BulletBase, T>;
    template<is_a_bullet T>
    struct BulletPool;
    template<is_a_bullet T>
    struct Emitter;
    enum class EmitterType { Down,
                             Radial };

    struct World;

    struct Sprite {
    public:
        Sprite(const std::string &animation_file, Vector2 &&position, uint8_t amount_frames)
            : texture{LoadTextureFromImage(raylib::LoadImage(animation_file))},
              position{position},
              current_frame_{0},
              frame_rect{0.0f, 0.0f, static_cast<float>(texture.width) / amount_frames, static_cast<float>(texture.height)},
              amount_frames_{amount_frames} {};
        Sprite(const Texture2D &texture, Vector2 &&position, uint8_t amount_frames)
            : texture{texture},
              position{position},
              current_frame_{0},
              frame_rect{0.0f, 0.0f, static_cast<float>(texture.width) / amount_frames, static_cast<float>(texture.height)},
              amount_frames_{amount_frames} {};

        Vector2 position;
        Texture2D texture;
        Rectangle frame_rect;
        float inner_timer{0.0f};

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

    struct Enemy {
        using BehaviorFunction = std::function<void(Enemy &, double)>;

    public:
        Enemy(Sprite animation, BehaviorFunction behavior, Vector2 position) : animation{animation}, behavior{std::move(behavior)}, position{position} {};
        Sprite animation;
        BehaviorFunction behavior;
        Vector2 position;
    };

    struct BulletBase {
    };

    struct NormalBullet : BulletBase {
        using BulletUpdateFunction = std::function<bool(float time, NormalBullet &bullet, Sprite &sprite, Rectangle &world_bounds)>;

    public:
        bool active{false};
        Vector2 position{0.0f, 0.0f};
        Vector2 direction{0.0f, 0.0f};
        float timer{0.0f};
        BulletUpdateFunction update_function{[&](float time, BulletBase &bullet, Sprite &sprite, Rectangle &world_bounds) -> bool { return false; }};
        NormalBullet() : active{false}, position{0.0f, 0.0f}, direction{0.0, 0.0f}, timer{0.0f}, update_function{[&](float time, BulletBase &bullet, Sprite &sprite, Rectangle &world_bounds) -> bool { return false; }} {};
        explicit NormalBullet(Vector2 position, Vector2 direction, BulletUpdateFunction update_function)
            : position{position}, direction{direction}, update_function{std::move(update_function)} {};
    };


    template<is_a_bullet T>
    struct BulletPool {
        using BulletUpdateFunction = std::function<bool(float time, T &bullet, Sprite &sprite, Rectangle &world_bounds)>;

    public:
        explicit BulletPool(const uint64_t amount) : pool(amount){};
        int first_available_index{0};
        std::vector<T> pool{1000};
        void add(T &&bullet);
        void removeAt(uint64_t index) {
            if (index < pool.size() && index < first_available_index) {
                for (int i = index; i < first_available_index; i++) {
                    pool[i] = pool[i + 1];
                }
                first_available_index--;
            }
        }
    };
    template<is_a_bullet T>
    void BulletPool<T>::add(T &&bullet) {
        if (first_available_index < pool.size()) {
            pool[first_available_index] = bullet;
            first_available_index++;
        }
    }

    template<>
    void BulletPool<NormalBullet>::add(NormalBullet &&bullet) {
        if (first_available_index < pool.size()) {
            auto &bullet_at_index = pool[first_available_index];
            bullet_at_index.active = true;
            bullet_at_index.timer = 0.0f;
            bullet_at_index.position = bullet.position;
            bullet_at_index.direction = bullet.direction;
            bullet_at_index.update_function = bullet.update_function;
            first_available_index++;
        }
    }

    struct World {
        Sprite player;
    };

    template<is_a_bullet T>
    struct Emitter {
        using BulletUpdateFunction = std::function<bool(float time, T &bullet, Sprite &sprite, Rectangle &world_bounds)>;
        Vector2 position;
        double delay_between_shots;
        EmitterType type;
        void emit(const int amount, const World& world);
    };

    template<>
    void Emitter<NormalBullet>::emit(const int amount, const World& world) {
        for (int i = 0; i < amount; i++) {

        }
    }


}// namespace basilevs

#endif//BASILEVS_CORE_H
