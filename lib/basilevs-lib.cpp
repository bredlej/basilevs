//
// Created by geoco on 21.04.2021.
//
#include <assets.h>
#include <basilevs-lib.h>
namespace basilevs {
    void Game::run(raylib::Window &window, raylib::AudioDevice &audio) {
        using namespace sml;
        using namespace state;
        GameDefinition game;

        //sm.process_event(events::Init{});
        //sm.process_event(events::Run{});
        //sm.process_event(events::Stop{});
        game.run(window, audio);
    }
}// namespace basilevs

/*
void GameDefinition::load_sprites_() const {
    sprite_template_map = {
            {basilevs::EntityType::Player, Sprite(texture_map.at(basilevs::EntityType::Player), raylib::Vector2{64, 90}, 7)},
            {basilevs::EntityType::Enemy, Sprite(texture_map.at(basilevs::EntityType::Enemy), raylib::Vector2(70, 30), 1)},
            {basilevs::EntityType::BulletRound,Sprite(texture_map.at(basilevs::EntityType::BulletRound), raylib::Vector2(50, 50), 1)},
            {basilevs::EntityType::BulletPlayer, Sprite(texture_map.at(basilevs::EntityType::BulletPlayer), raylib::Vector2(50, 50), 1)}};
}
*/

namespace emitter {
    enum class Type { ShootEverySecond,
                      ShootSpiral,
                      ShootNormal,
                      ShootCircular };

    /*static constexpr auto shoot_every_second = [](const float time, Emitter &emitter, World &world) {
        if (emitter.is_active) {
            if (emitter.last_shot > emitter.delay_between_shots) {

                world.enemy_bullets.add(NormalBullet{emitter.position, Vector2{0.0, 1.0f}, basilevs::bullet::bullet_fly_forward});
                emitter.last_shot = 0.0f;
            }
            emitter.last_shot += time;
        }
    };

    static constexpr auto shoot_spiral = [](const float time, Emitter &emitter, World &world) {
        if (emitter.is_active) {
            if (emitter.last_shot > emitter.delay_between_shots) {
                //   for (int i = 0 ; i < 4; i++) {
                emitter.angle += 15.0f;
                if (emitter.angle > 360.0f) {
                    emitter.angle = 0.0f;
                }
                world.enemy_bullets.add(NormalBullet{emitter.position, Vector2Rotate({0.0, 1.0f}, emitter.angle), basilevs::bullet::bullet_fly_spiral});
                //  PlaySound(emitter.sound);
                world.enemy_bullets.add(NormalBullet{emitter.position, Vector2Rotate({0.0, 1.0f}, emitter.angle + 90), basilevs::bullet::bullet_fly_spiral});
                //  PlaySound(emitter.sound);

                world.enemy_bullets.add(NormalBullet{emitter.position, Vector2Rotate({0.0, 1.0f}, emitter.angle + 180), basilevs::bullet::bullet_fly_spiral});
                //  PlaySound(emitter.sound);
                world.enemy_bullets.add(NormalBullet{emitter.position, Vector2Rotate({0.0, 1.0f}, emitter.angle + 270), basilevs::bullet::bullet_fly_spiral});
                // PlaySound(emitter.sound);
                // }
                emitter.last_shot = 0.0f;
            }
            emitter.last_shot += time;
        }
    };

    static constexpr auto player_normal_shoot = [](const float time, Emitter &emitter, World &world) {
        if (emitter.is_active) {
            if (emitter.last_shot > emitter.delay_between_shots) {

                world.player_bullets.add(NormalBullet{Vector2Add(emitter.position, Vector2{-5.0f, -1.0f}), Vector2{0.0f, -1.0f}, basilevs::bullet::bullet_fly_forward_fast});
                world.player_bullets.add(NormalBullet{Vector2Add(emitter.position, Vector2{5.0f, -1.0f}), Vector2{0.0f, -1.0f}, basilevs::bullet::bullet_fly_forward_fast});
                world.player_bullets.add(NormalBullet{emitter.position, Vector2{-1.0f, -1.0f}, basilevs::bullet::bullet_fly_forward_fast});
                world.player_bullets.add(NormalBullet{emitter.position, Vector2{1.0f, -1.0f}, basilevs::bullet::bullet_fly_forward_fast});
                // PlaySound(emitter.sound);
                emitter.last_shot = 0.0f;
            }
        }
        emitter.last_shot += time;
    };

    static constexpr auto shoot_circular = [](const float time, Emitter &emitter, World &world) {
        if (emitter.is_active) {
            if (emitter.last_shot > emitter.delay_between_shots) {
                auto direction = Vector2{0.0f, 1.0f};
                for (int i = 1; i < 60; i++) {
                    //PlaySound(emitter.sound);
                    world.enemy_bullets.add(NormalBullet{emitter.position, direction, basilevs::bullet::bullet_fly_forward});
                    direction = Vector2Rotate(direction, 360 / i);
                }
                emitter.last_shot = 0.0f;
            }
            emitter.last_shot += sin(time);
        }
    };*/
}// namespace emitter


void GameDefinition::initialize_world_() {

    world.background = basilevs::initialize_background(textures_);
    world.player = basilevs::initialize_player(textures_);
    world.enemies = basilevs::initialize_enemies(textures_);

    /* auto player = BasilevsPlayer{};
    player.sprite = sprite_template_map.at(basilevs::EntityType::Player);
    player.emitter = Emitter{{0.0f, 0.0f}, 0.023f, emitter::player_normal_shoot};
    world = World{
            player,
            Background("assets/basilevs_bg_001.png", 6),
            Rectangle{-10.0f, -10.0f, config::kFrameWidth + 10.0f, config::kFrameHeight + 10.0f},
            BulletPool<NormalBullet>(1000L), BulletPool<NormalBullet>(500L)};

    auto radial_emitter = Emitter{sprite_template_map.at(basilevs::EntityType::Enemy).position, 1.5f, emitter::shoot_circular};
    auto spiral_emitter = Emitter{sprite_template_map.at(basilevs::EntityType::Enemy).position, 0.1f, emitter::shoot_spiral};

    world.enemy_spawns = std::list<Spawn>({
            {1.0f, Vector2{60.0, 10.0}, sprite_template_map.at(basilevs::EntityType::Enemy), basilevs::enemy::behavior_sinusoidal, radial_emitter},
            {3.0f, Vector2{60.0, 30.0}, sprite_template_map.at(basilevs::EntityType::Enemy), basilevs::enemy::behavior_sinusoidal, spiral_emitter},
    });

    world.enemies_on_screen = std::vector<Enemy>{};*/
}

void GameDefinition::initialize() {
    sm.process_event(state::events::Init{});
    textures_ = assets::load_textures_level_1();
    initialize_world_();
    SetTargetFPS(60);
    sm.process_event(state::events::Run{});
}

void GameDefinition::loop_(std::chrono::duration<double> duration) {
    world.background->update(duration.count(), world);
    world.player->update(duration.count(), world);
    world.enemies->update(duration.count(), world);
    render_();
}

void GameDefinition::run(raylib::Window &window, raylib::AudioDevice &audio) {
    if (!state.is_initialized) {
        initialize();
    }
    std::chrono::duration<double> loop_duration = std::chrono::steady_clock::now() - std::chrono::steady_clock::now();
    while (!window.ShouldClose()) {
        auto now = std::chrono::steady_clock::now();
        loop_(loop_duration);
        loop_duration = std::chrono::steady_clock::now() - now;
    }
}

void GameDefinition::render_() {
    BeginDrawing();
    ClearBackground(config::colors::kBackground);
    basilevs::render_to_texture(render_target_, world, textures_);
    basilevs::render_to_screen(render_target_, world);
    EndDrawing();
}
