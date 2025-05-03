//
// Created by geoco on 09.04.2021.
//

#ifndef BASILEVS_CORE_H
#define BASILEVS_CORE_H

#include "imgui/imgui.h"

#include <behaviours/background.h>
#include <behaviours/enemy.h>
#include <behaviours/player.h>
#include <config.h>
#include <raylib.h>
#include <world.h>

namespace basilevs
{
    using namespace components;

    namespace initialization
    {
        enum class AnimationDirection {
            Vertical,
            Horizontal
        };

        static void setup_sprite(Sprite &sprite_component, const Core::TextureCache &texture_cache, const assets::TextureId texture_id, const uint32_t amount_frames, const AnimationDirection animation_direction)
        {
            const Texture2D texture = texture_cache[assets::texture_id_to_string[texture_id]];
            const int texture_width = texture.width;
            const int texture_height = texture.height;

            sprite_component.current_visible_frame = std::rand() % amount_frames;
            sprite_component.texture = texture_id;
            sprite_component.amount_frames = amount_frames;
            sprite_component.texture_width_px = texture_width;
            sprite_component.texture_height_px = texture_height;
            sprite_component.fps_speed = 12;
            if (AnimationDirection::Horizontal == animation_direction) { sprite_component.frame_rect = {0.0f, 0.0f, static_cast<float>(texture_width) / static_cast<float>(sprite_component.amount_frames), static_cast<float>(texture_height)}; } else { sprite_component.frame_rect = {0.0, static_cast<float>(texture_height) - (static_cast<float>(texture_height) / static_cast<float>(amount_frames)), static_cast<float>(texture_width), static_cast<float>(texture_height) / static_cast<float>(amount_frames)}; }
        }

        static void setup_enemy_sprite(Sprite &sprite_component, const Core::TextureCache &texture_cache, const behaviours::enemy::EnemyDefinition &enemy_definition)
        {
            setup_sprite(sprite_component, texture_cache, enemy_definition.texture, enemy_definition.amount_frames, AnimationDirection::Horizontal);
            sprite_component.current_state = StateEnum::IDLE;
            sprite_component.state_animations = enemy_definition.animations;
        }

        static void setup_movement(Movement &movement, const float x, const float y, const float speed)
        {
            movement.position = Vector2{x, y};
            movement.speed = speed;
        }

        static void setup_activation(Activation &activation, const double time)
        {
            activation.activate_after_seconds = time;
            activation.is_active = false;
        }

        static void setup_collision(Collision &collision, const float radius, const Vector2 &center)
        {
            collision.bounds.radius = radius;
            collision.bounds.center = center;
            collision.is_collidable = true;
        }


        static void create_player_entt(const Core::TextureCache &texture_cache, entt::registry &registry)
        {
            BlueprintEntt blueprintEntt(registry);
            Sprite sprite;
            setup_sprite(sprite, texture_cache, assets::TextureId::Player, 7, AnimationDirection::Horizontal);
            Movement movement;
            setup_movement(movement, 70.0f, 100.0f, 50.0f);
            Collision collision;
            setup_collision(collision, 3.0f, Vector2{17.0f, 18.0f});

            auto player = blueprintEntt.builder()
                                       .with<Player>()
                                       .with<Sprite>(sprite)
                                       .with<Movement>(movement)
                                       .with<Collision>(collision)
                                       .with<Emission>()
                                       .with<Health>()
                                       .with<TWorld::PlayerStateComponent>()
                                       .with<UpdateFlag>()
                                       .with<UpdateFunction>(behaviours::player::default_behaviour_new)
                                       .with<Activation>(true)
                                       .build();

            registry.ctx().emplace<Player>(player);
        }


        static entt::entity create_enemy(entt::registry &registry, const double seconds_until_spawns, const Vector2 &position, const behaviours::enemy::EnemyDefinition &enemy_definition)
        {
            BlueprintEntt enemy(registry);

            return enemy.builder()
                        .with<Sprite>()
                        .with<Movement>(position, Vector2{}, enemy_definition.speed)
                        .with<MovementPath>(enemy_definition.path)
                        .with<Activation>(false, seconds_until_spawns)
                        .with<TimeCounter>(0.0f)
                        .with<Emission>(0.0f)
                        .with<Collision>(enemy_definition.collision_center_offset, enemy_definition.collision_radius, true)
                        .with<Health>(enemy_definition.health)
                        .with<UpdateFlag>()
                        .with<UpdateFunction>(enemy_definition.behaviour)
                        .with<StateMachine<state_handling::transitions::EnemyPossibleStates, state_handling::StatefulObject>>()
                        .build();
        }

        static void create_enemy_with_sprite_entt(entt::registry &registry, const double seconds_until_spawns, const Core::TextureCache &texture_cache, const Vector2 &position, const behaviours::enemy::EnemyDefinition &enemy_definition)
        {
            const auto enemy = create_enemy(registry, seconds_until_spawns, position, enemy_definition);
            auto &sprite = registry.get<Sprite>(enemy);
            setup_enemy_sprite(sprite, texture_cache, enemy_definition);
        }

        static void create_background(const Core::TextureCache &texture_cache, entt::registry &registry)
        {
            Sprite sprite;
            setup_sprite(sprite, texture_cache, assets::TextureId::Background_Level_1, 6, AnimationDirection::Vertical);

            BlueprintEntt background(registry);
            background.builder()
                      .with<Background>()
                      .with<Sprite>(sprite)
                      .with<Activation>(true, 0.0)
                      .with<UpdateFlag>()
                      .with<UpdateFunction>(behaviours::background::level1_background_update);
        }
    }// namespace initialization

    namespace game_state
    {\
        static void update(const auto time_since_last_update, entt::registry &registry) { registry.view<UpdateFlag, UpdateFunction, Activation>().each([&registry, time_since_last_update](auto entity, const UpdateFlag &flag, UpdateFunction &func, const Activation &activation) { if (flag.enabled) { func.operator()(time_since_last_update.count(), entity, registry); } }); }
    }// namespace game_state

    namespace rendering
    {
        static void render_player(RenderTexture &render_target, const entt::registry &registry, const Core::TextureCache &texture_cache)
        {

            const entt::entity player = registry.ctx().get<Player>().entity;
            const auto &sprite_component = registry.get<Sprite>(player);
            const auto &movement_component = registry.get<Movement>(player);

            const Texture &texture = texture_cache[assets::texture_id_to_string[sprite_component.texture]];

            DrawTextureRec(texture, sprite_component.frame_rect, movement_component.position, WHITE);
        }

        static bool is_render_allowed_for_state(const TWorld::EnemyStateComponent &state) { return !state.state_machine.is(boost::sml::X) && !state.state_machine.is(state_handling::declarations::kInitState); }

        static void render_enemy(const Core::TextureCache &textures, const Sprite &sprite, const Movement &movement, const TWorld::EnemyStateComponent &state) { if (is_render_allowed_for_state(state)) {
            DrawTextureRec(textures[assets::texture_id_to_string[sprite.texture]], sprite.frame_rect, movement.position, WHITE);
        } }

        static void render_enemies(RenderTexture &render_target, const entt::registry &registry, const Core::TextureCache &texture_cache) { registry.view<TWorld::EnemyStateComponent, Sprite, Movement>().each([&texture_cache](const auto entity, const auto enemy_state, const auto &sprite, const auto movement)
        {
            DrawRectangle(movement.position.x, movement.position.y, sprite.frame_rect.width, sprite.frame_rect.height, MAGENTA);
            render_enemy(texture_cache, sprite, movement, enemy_state);
            DrawText(TextFormat("%d",entity), movement.position.x, movement.position.y, 10, GREEN);
        }); }

        template<typename POOL_TYPE>
        static void render_bullets(RenderTexture &render_target, const entt::registry &registry, const Core::TextureCache &texture_cache)
        {
            for (auto &player_bullet_pool = registry.ctx().get<POOL_TYPE>().active_entities; auto bullet_entity : player_bullet_pool) {
                const auto &sprite = registry.get<Sprite>(bullet_entity);
                const auto &movement = registry.get<Movement>(bullet_entity);
                DrawTextureRec(texture_cache[assets::texture_id_to_string[sprite.texture]], sprite.frame_rect, movement.position, WHITE);
            }
        }

        static void render_background(RenderTexture &render_target, const entt::registry &registry, const Core::TextureCache &texture_cache)
        {
            registry.view<Background, Sprite>().each([&texture_cache](const entt::entity, const Background, const Sprite &sprite)
            {
                const auto &texture = texture_cache[assets::texture_id_to_string[sprite.texture]];
                DrawTextureRec(texture, sprite.frame_rect, {0, 0}, GRAY);
            });
        }


        /*
         * Renders the current game state into a texture.
         *
         * The textures dimension is declared in config.h, in this case something small like 160x144px.
         * After all contents are rendered, the texture itself will be rendered upscaled to the users screen dimensions in another function below.
         */
        static void render_to_texture(RenderTexture &render_target, const Core &core)
        {
            BeginTextureMode(render_target);
            ClearBackground(config::colors::kBackground);
            render_background(render_target, core.registry, core.texture2d_cache);
            render_player(render_target, core.registry, core.texture2d_cache);
            render_enemies(render_target, core.registry, core.texture2d_cache);
            render_bullets<PlayerBullets>(render_target, core.registry, core.texture2d_cache);
            render_bullets<EnemyBullets>(render_target, core.registry, core.texture2d_cache);

            EndTextureMode();
        }

        /*
         * This function renders the games current frame upscaled to actual screen dimensions, along with some UI elements
         */
        static void render_to_screen(const RenderTexture &render_target, const Core &core)
        {
            DrawTexturePro(render_target.texture,
                           Rectangle{0.0f, 0.0f, static_cast<float>(render_target.texture.width), static_cast<float>(-render_target.texture.height)},
                           Rectangle{0.0f, 0.0f, static_cast<float>(config::kScreenWidth), static_cast<float>(config::kScreenHeight)},
                           Vector2{0, 0},
                           0.0f,
                           WHITE);
            auto amount_bullets = 0;
            core.registry.view<TWorld::BulletStateComponent, Activation>().each([&amount_bullets](const auto entity, const auto &bullet_state, const auto &activation) { amount_bullets++; });
            DrawFPS(5, 5);
        }
    }// namespace rendering

    namespace collision_checking
    {
        /*
         * Helper struct which holds references to bullet components used in collision checking
         */
        struct CollisionCheckBulletComponents {
            std::vector<Movement> &movements;
            std::vector<Collision> &collisions;
            std::vector<TWorld::BulletStateComponent> &states;
            std::vector<Damage> &damages;
        };


        static bool is_enemy_collidable(const entt::entity enemy_entity, const entt::registry &registry)
        {
            const auto [is_active, activate_after_seconds] = registry.get<Activation>(enemy_entity);
            return is_active;
        }

        static void destroy_bullet(TWorld::BulletStateComponent &bullet_state) { bullet_state.state_machine.process_event(state_handling::events::DestroyEvent()); }

        static void handle_collision(TWorld::EnemyStateComponent &enemy_state, TWorld::BulletStateComponent &bullet_state)
        {
            enemy_state
                    .state_machine
                    .process_event(state_handling::events::DamageEvent());

            destroy_bullet(bullet_state);
        }

        static Vector2 get_collision_center(const Movement &movement, const Collision &collision) { return Vector2Add(movement.position, collision.bounds.center); }

        static float get_radius(const Collision &collision) { return collision.bounds.radius; }
        /*
         * Handle enemies colliding with bullets shot by the player
         */
        static void player_bullets_with_enemies(entt::registry &registry)
        {
            const auto grid = registry.ctx().get<SpatialGrid>();
            registry.view<Movement, Collision, TWorld::BulletStateComponent, PlayerBullet, Activation>().each([&registry, &grid](const auto entity, const auto &bullet_movement, const auto &bullet_collision, auto &bullet_state, const auto &bullet_tag, const auto &activation)
            {
                for (auto other : grid.query(bullet_movement.position.x, bullet_movement.position.y)) {
                    if (!registry.any_of<TWorld::EnemyStateComponent>(other) && !is_enemy_collidable(other, registry)) continue;
                    const Vector2 &bullet_center = get_collision_center(bullet_movement, bullet_collision);
                    const float &bullet_radius = get_radius(bullet_collision);
                    const auto &enemy_movement = registry.get<Movement>(other);
                    const auto &enemy_collision = registry.get<Collision>(other);
                    if (TWorld::EnemyStateComponent *enemy_state = registry.try_get<TWorld::EnemyStateComponent>(other)) {
                        const Vector2 &enemy_center = get_collision_center(enemy_movement, enemy_collision);
                        if (const float &enemy_radius = get_radius(enemy_collision); CheckCollisionCircles(bullet_center, bullet_radius, enemy_center, enemy_radius)) { handle_collision(*enemy_state, bullet_state); }
                    }
                }
            });
        }

        /*
         * Handle player bullets colliding with other bullets.
         * Intended for the case when enemies are shooting swarms of bullets which can be destroyed when being shot at.
         */
        static void player_bullets_with_enemy_bullets(entt::registry &registry)
        {
            registry.view<PlayerBullet, TWorld::BulletStateComponent, Movement, Collision, Activation>().each([&](const entt::entity player_bullet_entity, const PlayerBullet &player_bullet, TWorld::BulletStateComponent &bullet_state, const Movement &bullet_movement, const Collision &bullet_collision, const Activation &activation)
            {
                const Vector2 &bullet_center = get_collision_center(bullet_movement, bullet_collision);
                registry.view<EnemyBullet, TWorld::BulletStateComponent, Movement, Collision, Activation>().each([&bullet_center, &bullet_collision, &bullet_state, &registry](const entt::entity enemy_bullet_entity, const EnemyBullet &enemy_bullet, TWorld::BulletStateComponent &enemy_bullet_state, const Movement &enemy_bullet_movement, const Collision &enemy_bullet_collision, const Activation &activation)
                {
                    if (const Vector2 &enemy_bullet_center = get_collision_center(enemy_bullet_movement, enemy_bullet_collision); CheckCollisionCircles(bullet_center, bullet_collision.bounds.radius, enemy_bullet_center, enemy_bullet_collision.bounds.radius)) {
                        destroy_bullet(bullet_state);
                        destroy_bullet(enemy_bullet_state);
                    }
                });
            });
        }

        static void damage_player(Health &player_health, const float damage) { player_health.hp -= damage; }

        /*
         * Handle player colliding with enemy bullets
         */
        static void enemy_bullets_with_player(entt::registry &registry)
        {
            const entt::entity player = registry.ctx().get<Player>().entity;
            const Movement player_movement = registry.get<Movement>(player);
            const Collision player_collision = registry.get<Collision>(player);
            auto &player_health = registry.get<Health>(player);
            const Vector2 &player_collision_center = Vector2Add(player_movement.position, player_collision.bounds.center);

            registry.view<EnemyBullet, TWorld::BulletStateComponent, Movement, Collision, Damage, Activation>().each([&player_health, &player_collision_center, &player_collision]
            (entt::entity bullet_entity, const EnemyBullet &enemy_bullet, TWorld::BulletStateComponent &bullet_state, const Movement &bullet_movement, const Collision &bullet_collision, const Damage bullet_damage, const Activation &activation)
                    {
                        const Vector2 &collision_center = get_collision_center(bullet_movement, bullet_collision);

                        if (const float &collision_radius = get_radius(bullet_collision); CheckCollisionCircles(collision_center, collision_radius, player_collision_center, player_collision.bounds.radius)) {
                            destroy_bullet(bullet_state);
                            damage_player(player_health, bullet_damage.value);
                        }
                    });
        }

        /*
         * Here we check if the player, enemies and bullets collide with each other and do some logic if they do.
         */
        static void collision_checks(entt::registry &registry)
        {
            auto &grid = registry.ctx().get<SpatialGrid>();
            grid.clear();
            registry.view<Movement, Sprite, Activation>().each([&grid](const auto entity, const Movement &movement, const Sprite &sprite, const Activation &activation)
            {
                if (activation.is_active) {
                    grid.insert(entity, movement.position.x + sprite.frame_rect.width * 0.5f, movement.position.y + sprite.frame_rect.height * 0.5f, sprite.frame_rect.width * 0.5f, sprite.frame_rect.height * 0.5f);
                }
            });
            player_bullets_with_enemies(registry);
            //player_bullets_with_enemy_bullets(registry);
            //enemy_bullets_with_player(registry);
        }
    }// namespace collision_checking

    namespace memory
    {
        static bool is_bullet_outside_frame(const Movement &bullet_movement, const Sprite &sprite, const Frame &frame)
        {
            const auto [x, y] = bullet_movement.position;
            const auto [bx, by] = sprite.bounds;

            return !CheckCollisionRecs(Rectangle(x, y, bx, by), frame.bounds);
        }

        /*
         * Deactivates bullets which are outside the visible frame by a given margin.
         * Those bullets go into a "destroyed" state.
         */
        static void destroy_bullets_outside_frame(entt::registry &registry) { registry.view<TWorld::BulletStateComponent, Movement, Sprite>().each([&registry](const auto entity, TWorld::BulletStateComponent &bullet_state, const auto &movement, const auto &sprite) { if (is_bullet_outside_frame(movement, sprite, registry.ctx().get<Frame>())) { bullet_state.state_machine.process_event(state_handling::events::DestroyEvent()); } }); }

        static void remove_destroyed_bullets(entt::registry &registry,
                                             std::vector<entt::entity> &active_entities,
                                             std::vector<entt::entity> &free_entities)
        {
            for (size_t i = 0; i < active_entities.size(); ++i) {
                entt::entity e = active_entities[i];

                if (auto &st = registry.get<TWorld::BulletStateComponent>(e); st.state_machine.is(boost::sml::X)) {
                    registry.remove<Activation, UpdateFlag>(e);

                    std::swap(active_entities[i], active_entities.back());
                    active_entities.pop_back();
                    free_entities.push_back(e);

                    --i;
                }
            }
        }


        /*
         * Does some reindexing of inactive bullets to make room for new ones
         */
        static void cleanup_bullet_pools(entt::registry &registry)
        {
            destroy_bullets_outside_frame(registry);
            auto &[bullets_player_free, bullets_player_active] = registry.ctx().get<PlayerBullets>();
            remove_destroyed_bullets(registry, bullets_player_active, bullets_player_free);

            auto &[bullets_enemy_free, bullets_enemy_active] = registry.ctx().get<EnemyBullets>();
            remove_destroyed_bullets(registry, bullets_enemy_active, bullets_enemy_free);
        }
    }// namespace memory

    namespace io
    {
        static void handle_player_input(Core &core)
        {
            auto &input = core.registry.ctx().get<input::UserInput<input::PlayerInput>>();
            register_input({KEY_A, KEY_LEFT}, input::PlayerInput::Left, input);
            register_input({KEY_D, KEY_RIGHT}, input::PlayerInput::Right, input);
            register_input({KEY_W, KEY_UP}, input::PlayerInput::Up, input);
            register_input({KEY_S, KEY_DOWN}, input::PlayerInput::Down, input);
            register_input({KEY_SPACE}, input::PlayerInput::Shoot, input);
        }
    }// namespace io

    namespace audio
    {
        static void play_sounds(std::vector<assets::SoundId> &sounds_queue, const Core::SoundCache &sounds)
        {
            for (assets::SoundId &sound : sounds_queue) { PlaySound(sounds[assets::sound_id_to_string[sound]]); }
            sounds_queue.clear();
        }
    }// namespace audio
}// namespace basilevs

#endif//BASILEVS_CORE_H