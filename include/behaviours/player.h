//
// Created by geoco on 22.06.2021.
//

#ifndef BASILEVS_PLAYER_H
#define BASILEVS_PLAYER_H
#include <config.h>
#include <world.h>

/*
 * Describes how to update the player object and its bullets
 * Since the player and its bullets must have a world they reside in, the components inside their respective UpdateFunctions must be the same as declared in the blueprint aliases (PlayerType, BulletPool) in world.h
 */
namespace behaviours
{
    using UpdateFunction = std::function<void(double, entt::entity, entt::registry &)>;

    namespace bullet
    {
        static constexpr auto player_bullet_1 = [](const double time, const entt::entity &entity, entt::registry &registry)
        {
            auto &movement = registry.get<components::Movement>(entity);
            movement.position.x += movement.direction.x * static_cast<float>(time) * movement.speed;
            movement.position.y += movement.direction.y * static_cast<float>(time) * movement.speed;
        };
    }// namespace bullet

    namespace player
    {
        /*
         * Shoots four bullets in a `\||/` pattern
         */
        static constexpr auto shoot_quadruple = [](const double time, entt::registry &registry, const UpdateFunction &bullet_function)
        {
            const auto &player = registry.ctx().get<components::Player>().entity;
            constexpr auto speed = 100;

            static auto emit = [&](const UpdateFunctionType &bullet_function_type, const auto movement_component, const auto position, const auto direction, const auto rotation)
            {
                components::Sprite bullet_sprite;
                bullet_sprite.offset = Vector2{16.0f, 16.0f};
                bullet_sprite.texture = assets::TextureId::Player_Bullet;
                bullet_sprite.frame_rect = Rectangle(0, 0, 8, 8);

                components::Movement bullet_movement;
                bullet_movement.speed = speed;
                bullet_movement.position = movement_component.position;
                bullet_sprite.rotation_degrees = Vector2Angle({0.0f, 0.0f}, rotation);
                bullet_movement.position = Vector2Add(bullet_movement.position, position);
                bullet_movement.direction = direction;

                components::Collision bullet_collision;
                bullet_collision.bounds.center = Vector2{5.0f, 5.0f};
                bullet_collision.bounds.radius = 2.0f;
                bullet_collision.is_collidable = false;

                components::Damage bullet_damage;
                bullet_damage.value = 30.0f;

                BlueprintEntt(registry).builder()
                                       .with<TWorld::BulletStateComponent>()
                                       .with<components::UpdateFunction>(bullet_function)
                                       .with<components::Sprite>(bullet_sprite)
                                       .with<components::Movement>(bullet_movement)
                                       .with<components::Collision>(bullet_collision)
                                       .with<components::Damage>(bullet_damage)
                                       .with<components::PlayerBullet>();
            };

            auto &[last_emission_seconds] = registry.get<components::Emission>(player);
            const auto &movement = registry.get<components::Movement>(player);

            if (constexpr auto emit_every_seconds = 0.2; last_emission_seconds > emit_every_seconds) {
                last_emission_seconds = 0.0;
                emit(bullet_function, movement, Vector2{8.0, -4.0}, Vector2{0.0, -1.0}, Vector2{0.0f, 0.0f});
                emit(bullet_function, movement, Vector2{16.0, -4.0}, Vector2{0.0, -1.0}, Vector2{0.0f, 0.0f});
                emit(bullet_function, movement, Vector2{0.0, -4.0}, Vector2{-0.3, -1.0}, Vector2{1.0f, -0.3f});
                emit(bullet_function, movement, Vector2{24.0, -4.0}, Vector2{0.3, -1.0}, Vector2{1.0f, 0.3f});
            }
            last_emission_seconds += time;
        };

        static auto animation_update = [](components::Sprite &sprite)
        {
            sprite.fps_counter++;

            if (sprite.fps_counter >= (120 / sprite.fps_speed)) {
                sprite.fps_counter = 0;
                sprite.current_visible_frame++;

                if (sprite.current_visible_frame > sprite.amount_frames - 1) sprite.current_visible_frame = 0;

                sprite.frame_rect.x = static_cast<float>(sprite.current_visible_frame) * static_cast<float>(sprite.texture_width_px) / static_cast<float>(sprite.amount_frames);
            }
        };

        static auto default_behaviour_new = [](const double time, const entt::entity entity, entt::registry &registry)
        {
            static auto move_func = [&time](const input::UserInput<input::PlayerInput> &player_input, components::Movement &movement)
            {
                if (player_input[input::PlayerInput::Left]) { movement.position.x -= movement.speed * time; }
                if (player_input[input::PlayerInput::Right]) { movement.position.x += movement.speed * time; }
                if (player_input[input::PlayerInput::Up]) { movement.position.y -= movement.speed * time; }
                if (player_input[input::PlayerInput::Down]) { movement.position.y += movement.speed * time; }
            };

            static auto shoot = [&time, &entity, &registry](const input::UserInput<input::PlayerInput> player_input) { if (player_input[input::PlayerInput::Shoot]) { shoot_quadruple(time, registry, bullet::player_bullet_1); } };

            components::Sprite &sprite = registry.get<components::Sprite>(entity);
            components::Movement &movement = registry.get<components::Movement>(entity);
            components::Emission &emission = registry.get<components::Emission>(entity);

            animation_update(sprite);
            const auto input = registry.ctx().get<input::UserInput<input::PlayerInput>>();
            move_func(input, movement);
            shoot(input);
        };
    }// namespace player
}// namespace behaviours
#endif//BASILEVS_PLAYER_H