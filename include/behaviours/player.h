//
// Created by geoco on 22.06.2021.
//

#ifndef BASILEVS_PLAYER_H
#define BASILEVS_PLAYER_H
#include <world.h>
#include <config.h>
namespace behaviours {
    namespace bullet {
        static constexpr auto player_bullet_1 = [](const double time, TWorld &world, components::Sprite &sprite, components::Movement &movement, components::StateMachine<state::BulletState, state::StatefulObject> &state, components::TimeCounter &time_counter, components::Collision &collision, components::Damage &) {
          movement.position.x += movement.direction.x * static_cast<float>(time) * movement.speed;
          movement.position.y += movement.direction.y * static_cast<float>(time) * movement.speed;
        };
        using UpdateFunction = std::function<void(const double, TWorld &, components::Sprite &, components::Movement &, components::StateMachine<state::BulletState, state::StatefulObject> &, components::TimeCounter &time_counter, components::Collision &collision, components::Damage &)>;
    }// namespace bullet

    namespace player {

        static constexpr auto shoot_1 = [](TWorld &world, components::Emission &emitter, const components::Movement movement, const double time, const bullet::UpdateFunction &bullet_function) {
          const auto &player = world.player.get();
          constexpr auto emit_every_seconds = 0.2;
          constexpr auto speed = 100;
          static constexpr auto emit = [&](auto &world, const bullet::UpdateFunction &bullet_function, const auto movement_component, const auto position, const auto direction, const auto rotation)
          {
              auto bullet_blueprint = Blueprint(bullet::UpdateFunction(bullet_function));
              auto &sprite_component = get<components::Sprite>(bullet_blueprint);
              sprite_component.offset = raylib::Vector2{16.0f, 16.0f};
              sprite_component.texture = assets::TextureId::Player_Bullet;
              sprite_component.frame_rect = raylib::Rectangle(0, 0, 8, 8);

              auto &bullet_movement = get<components::Movement>(bullet_blueprint);
              bullet_movement.speed = speed;
              bullet_movement.position = movement_component.position;
              sprite_component.rotation_degrees = raylib::Vector2(0.0f, 0.0f).Angle(rotation);
              bullet_movement.position = bullet_movement.position.Add(position);
              bullet_movement.direction = direction;

              auto &bullet_collision = get<components::Collision>(bullet_blueprint);
              bullet_collision.bounds.center = raylib::Vector2{5.0f, 5.0f};
              bullet_collision.bounds.radius = 2.0f;
              bullet_collision.is_collidable = false;

              world.player_bullets.add(bullet_blueprint);
          };

          if (emitter.last_emission > emit_every_seconds) {
              emitter.last_emission = 0.0;
              emit (world, bullet_function, movement, raylib::Vector2{8.0, -4.0}, raylib::Vector2{0.0, -1.0}, raylib::Vector2{0.0f, 0.0f});
              emit (world, bullet_function, movement, raylib::Vector2{16.0, -4.0}, raylib::Vector2{0.0, -1.0}, raylib::Vector2{0.0f, 0.0f});
              emit (world, bullet_function, movement, raylib::Vector2{0.0, -4.0}, raylib::Vector2{-0.3, -1.0}, raylib::Vector2{1.0f, -0.3f});
              emit (world, bullet_function, movement, raylib::Vector2{24.0, -4.0}, raylib::Vector2{0.3, -1.0}, raylib::Vector2{1.0f, 0.3f});
          }
          emitter.last_emission += time;
        };

        static constexpr auto frame_update = [](components::Sprite &sprite) {
            sprite.fps_counter++;

            if (sprite.fps_counter >= (60 / sprite.fps_speed)) {
                sprite.fps_counter = 0;
                sprite.current_visible_frame++;

                if (sprite.current_visible_frame > sprite.amount_frames - 1) sprite.current_visible_frame = 0;

                sprite.frame_rect.x = static_cast<float>(sprite.current_visible_frame) * static_cast<float>(sprite.texture_width_px) / static_cast<float>(sprite.amount_frames);
            }
        };

        static constexpr auto kPlayerNormalBehaviour = [](const double time, TWorld &world, components::Sprite &sprite, components::Movement &movement, components::Emission &emission, components::Collision &collision, components::Health &health, TWorld::PlayerStateComponent &state) {
            static constexpr auto move = [] (const double time, TWorld &world, components::Movement &movement) {
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
            static constexpr auto shoot = [] (const double time, TWorld &world, components::Sprite &sprite, components::Movement &movement, components::Emission &emission) {
                if (world.player_input[input::PlayerInput::Shoot]) {
                    shoot_1(world, emission, movement, time, bullet::player_bullet_1);
                }
            };
            frame_update(sprite);
            move(time, world, movement);
            shoot(time, world, sprite, movement, emission);
        };
        using UpdateFunction =
                std::function<void(
                        const double,
                        TWorld &,
                        components::Sprite &,
                        components::Movement &,
                        components::Emission &,
                        components::Collision &,
                        components::Health &,
                        TWorld::PlayerStateComponent &)>;
    }// namespace player
}
#endif//BASILEVS_PLAYER_H
