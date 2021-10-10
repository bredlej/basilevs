//
// Created by geoco on 10.10.2021.
//
#include <initialization.h>
namespace initialization
{
    static void setup_sprite(Sprite &sprite_component, const std::vector<Texture2D> &textures, const assets::TextureId texture_id, const uint32_t amount_frames, const AnimationDirection animation_direction)
    {
        int texture_width = textures[static_cast<int>(texture_id)].width;
        int texture_height = textures[static_cast<int>(texture_id)].height;
        sprite_component.current_visible_frame = std::rand() % amount_frames;
        sprite_component.texture = texture_id;
        sprite_component.amount_frames = amount_frames;
        sprite_component.texture_width_px = texture_width;
        sprite_component.texture_height_px = texture_height;
        sprite_component.fps_speed = 12;
        if (AnimationDirection::Horizontal == animation_direction) {
            sprite_component.frame_rect = {0.0f, 0.0f, static_cast<float>(texture_width) / static_cast<float>(sprite_component.amount_frames), static_cast<float>(texture_height)};
        } else {
            sprite_component.frame_rect = {0.0, static_cast<float>(texture_height) - (static_cast<float>(texture_height) / static_cast<float>(amount_frames)), static_cast<float>(texture_width), static_cast<float>(texture_height) / static_cast<float>(amount_frames)};
        }
    }

    static void setup_enemy_sprite(Sprite &sprite_component, const std::vector<Texture2D> &textures, const behaviours::enemy::EnemyDefinition &enemy_definition)
    {
        setup_sprite(sprite_component, textures, enemy_definition.texture, enemy_definition.amount_frames, AnimationDirection::Horizontal);
        sprite_component.current_state = StateEnum::IDLE;
        sprite_component.state_animations = enemy_definition.animations;
    }

    static void setup_movement(Movement &movement, const float x, const float y, const float speed)
    {
        movement.position = raylib::Vector2{x, y};
        movement.speed = speed;
    }

    static void setup_activation(Activation &activation, const double time)
    {
        activation.activate_after_seconds = time;
        activation.is_active = false;
    }

    static void setup_collision(Collision &collision, const float radius, const raylib::Vector2 &center)
    {
        collision.bounds.radius = radius;
        collision.bounds.center = center;
        collision.is_collidable = true;
    }

    TWorld::PlayerType create_player(const std::vector<Texture2D> &textures)
    {
        TWorld::PlayerType player = Blueprint(behaviours::player::UpdateFunction(behaviours::player::default_behaviour));
        setup_sprite(get<Sprite>(player), textures, assets::TextureId::Player, 7, AnimationDirection::Horizontal);
        setup_movement(get<Movement>(player), 70.0f, 100.0f, 50.0f);
        setup_collision(get<Collision>(player), 3.0f, raylib::Vector2{17.0f, 18.0f});

        return player;
    }

    TWorld::EnemyType create_enemy(const double seconds_until_spawns, const Vector2 &position, const behaviours::enemy::EnemyDefinition &enemy_definition)
    {
        TWorld::EnemyType enemy = Blueprint(behaviours::enemy::UpdateFunction(enemy_definition.behaviour));
        get<Movement>(enemy).position = position;
        get<Movement>(enemy).speed = enemy_definition.speed;
        get<TimeCounter>(enemy).elapsed_seconds = 0.0;
        get<Emission>(enemy).last_emission_seconds = 0.0f;
        get<MovementPath>(enemy).points = enemy_definition.path;
        get<Health>(enemy).hp = enemy_definition.health;
        setup_activation(get<Activation>(enemy), seconds_until_spawns);
        setup_collision(get<Collision>(enemy), enemy_definition.collision_radius, enemy_definition.collision_center_offset);

        return enemy;
    }

    TWorld::EnemyType create_enemy_with_sprite(const double seconds_until_spawns, const std::vector<Texture2D> &textures, const Vector2 &position, const behaviours::enemy::EnemyDefinition &enemy_definition)
    {
        TWorld::EnemyType enemy = create_enemy(seconds_until_spawns, position, enemy_definition);
        setup_enemy_sprite(get<Sprite>(enemy), textures, enemy_definition);

        return enemy;
    }

    TWorld::BackgroundType create_background(const std::vector<Texture2D> &textures)
    {
        TWorld::BackgroundType background = Blueprint(behaviours::background::UpdateFunction(behaviours::background::level1_background_update));
        setup_sprite(get<Sprite>(background), textures, assets::TextureId::Background_Level_1, 6, AnimationDirection::Vertical);

        return background;
    }
}// namespace initialization