//
// Created by geoco on 22.06.2021.
//

#ifndef BASILEVS_PLAYER_H
#define BASILEVS_PLAYER_H
#include <world.h>
#include <config.h>
namespace behaviours {
    namespace bullet {
        constexpr auto player_bullet_1 = [](const double time, TWorld &world, components::Sprite &sprite, components::Movement &movement, components::CollisionCheck &collisionCheck) {
          movement.position.x += movement.direction.x * static_cast<float>(time) * movement.speed;
          movement.position.y += movement.direction.y * static_cast<float>(time) * movement.speed;

          /*const auto &player = world.player.get();
          const auto player_movement = get<components::Movement>(player->components);
          if (movement.position.CheckCollision(player_movement.position.Add({16, 16}), 4.0)) {
              movement.speed = 0.0f;
          }*/
        };
        using UpdateFunction = std::function<void(const double, TWorld &, components::Sprite &, components::Movement &, components::CollisionCheck &)>;
    }// namespace bullet

    namespace player {

        constexpr auto shoot_1 = [](TWorld &world, components::Emission &emitter, const components::Movement movement, const double time, const bullet::UpdateFunction &bullet_function) {
          const auto &player = world.player.get();
          constexpr auto emit_every_seconds = 0.2;
          constexpr auto speed = 100;
          if (emitter.last_emission > emit_every_seconds) {

              emitter.last_emission = 0.0;
              // 1
              auto emitted_bullet_1 = Blueprint(bullet::UpdateFunction(bullet_function));
              auto &sprite_component_1 = get<components::Sprite>(emitted_bullet_1);
              sprite_component_1.offset = raylib::Vector2{16.0f, 16.0f};
              sprite_component_1.texture = assets::TextureId::Player_Bullet;
              sprite_component_1.frame_rect = raylib::Rectangle(0, 0, 8, 8);

              auto &bulletMovement_1 = get<components::Movement>(emitted_bullet_1);
              bulletMovement_1.speed = speed;
              bulletMovement_1.position = movement.position;
              bulletMovement_1.position = bulletMovement_1.position.Add({8.0, -4.0});
              bulletMovement_1.direction = Vector2{0.0, -1.0};
              //sprite_component.rotation = player_movement.position.Add({16.0, 16.0}).Angle(bullet_movement.position.Add({4.0, 4.0}));

              // 2
              auto emitted_bullet_2 = Blueprint(bullet::UpdateFunction(bullet_function));
              auto &sprite_component_2 = get<components::Sprite>(emitted_bullet_2);
              sprite_component_2.offset = raylib::Vector2{16.0f, 16.0f};
              sprite_component_2.texture = assets::TextureId::Player_Bullet;
              sprite_component_2.frame_rect = raylib::Rectangle(0, 0, 8, 8);

              auto &bulletMovement_2 = get<components::Movement>(emitted_bullet_2);
              bulletMovement_2.speed = speed;
              bulletMovement_2.position = movement.position;
              bulletMovement_2.position = bulletMovement_2.position.Add({16.0, -4.0});
              bulletMovement_2.direction = Vector2{0.0, -1.0};
              //sprite_component.rotation = player_movement.position.Add({16.0, 16.0}).Angle(bullet_movement.position.Add({4.0, 4.0}));

              // 3
              auto emitted_bullet_3 = Blueprint(bullet::UpdateFunction(bullet_function));
              auto &sprite_component_3 = get<components::Sprite>(emitted_bullet_3);
              sprite_component_3.offset = raylib::Vector2{16.0f, 16.0f};
              sprite_component_3.texture = assets::TextureId::Player_Bullet;
              sprite_component_3.frame_rect = raylib::Rectangle(0, 0, 8, 8);

              auto &bulletMovement_3 = get<components::Movement>(emitted_bullet_3);
              bulletMovement_3.speed = speed;
              bulletMovement_3.position = movement.position;
              sprite_component_3.rotation = raylib::Vector2(0.0f, 0.0f).Angle(raylib::Vector2{1.0f, -0.3f});
              bulletMovement_3.position = bulletMovement_3.position.Add({0.0, -4.0});
              bulletMovement_3.direction = Vector2{-0.3, -1.0};

              // 4
              auto emitted_bullet_4 = Blueprint(bullet::UpdateFunction(bullet_function));
              auto &sprite_component_4 = get<components::Sprite>(emitted_bullet_4);
              sprite_component_4.offset = raylib::Vector2{16.0f, 16.0f};
              sprite_component_4.texture = assets::TextureId::Player_Bullet;
              sprite_component_4.frame_rect = raylib::Rectangle(0, 0, 8, 8);

              auto &bulletMovement_4 = get<components::Movement>(emitted_bullet_4);
              bulletMovement_4.speed = speed;
              bulletMovement_4.position = movement.position;
              sprite_component_4.rotation = raylib::Vector2(0.0f, 0.0f).Angle(raylib::Vector2{1.0f, 0.3f});
              bulletMovement_4.position = bulletMovement_3.position.Add({24.0, -4.0});
              bulletMovement_4.direction = Vector2{0.3, -1.0};

              world.player_bullets.add(emitted_bullet_1);
              world.player_bullets.add(emitted_bullet_2);
              world.player_bullets.add(emitted_bullet_3);
              world.player_bullets.add(emitted_bullet_4);
              world.sounds_queue.emplace_back(assets::SoundId::NormalBullet);
          }
          emitter.last_emission += time;
        };

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
        constexpr auto shoot = [] (const double time, TWorld &world, components::Sprite &sprite, components::Movement &movement, components::Emission &emission) {
            if (world.player_input[input::PlayerInput::Shoot]) {
                shoot_1(world, emission, movement, time, bullet::player_bullet_1);
            }
        };
        constexpr auto kPlayerNormalBehaviour = [](const double time, TWorld &world, components::Sprite &sprite, components::Movement &movement, components::Emission &emission) {
            frame_update(sprite);
            move(time, world, movement);
            shoot(time, world, sprite, movement, emission);
        };
        using UpdateFunction = std::function<void(const double, TWorld &, components::Sprite &, components::Movement &, components::Emission &)>;
    }// namespace player
}
#endif//BASILEVS_PLAYER_H
