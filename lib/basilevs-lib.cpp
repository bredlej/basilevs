//
// Created by geoco on 21.04.2021.
//
#include <basilevs-lib.h>
#include <basilevs.h>

namespace basilevs {
    void Game::run(raylib::Window &window, raylib::AudioDevice &audio) {
        using namespace sml;
        GameDefinition game;
        sm<GameState> sm{game};
        sm.process_event(events::Init{});
        sm.process_event(events::Run{window, audio});
        sm.process_event(events::Stop{});
    }
}// namespace basilevs

void GameDefinition::load_textures_() const {
    texture_map = {
            {basilevs::EntityType::Player, LoadTextureFromImage(raylib::LoadImage("assets/player.png"))},
            {basilevs::EntityType::Enemy, LoadTextureFromImage(raylib::LoadImage("assets/enemy.png"))},
            {basilevs::EntityType::BulletPlayer, LoadTextureFromImage(raylib::LoadImage("assets/bullet8-002.png"))},
            {basilevs::EntityType::BulletRound, LoadTextureFromImage(raylib::LoadImage("assets/bullet8.png"))}};
}

void GameDefinition::load_sprites_() const {
    sprite_template_map = {
            {basilevs::EntityType::Player, basilevs::Sprite(texture_map.at(basilevs::EntityType::Player), raylib::Vector2{64, 90}, 7)},
            {basilevs::EntityType::Enemy, basilevs::Sprite(texture_map.at(basilevs::EntityType::Enemy), raylib::Vector2(70, 30), 1)},
            {basilevs::EntityType::BulletRound, basilevs::Sprite(texture_map.at(basilevs::EntityType::BulletRound), raylib::Vector2(50, 50), 1)},
            {basilevs::EntityType::BulletPlayer, basilevs::Sprite(texture_map.at(basilevs::EntityType::BulletPlayer), raylib::Vector2(50, 50), 1)}};
}

void GameDefinition::load_sounds_() const {
    sound_map = {{basilevs::EntityType::BulletRound, LoadSound("assets/bullet.wav")}};
}

template<>
void basilevs::BulletPool<basilevs::NormalBullet>::add(NormalBullet &&bullet) {
    if (first_available_index < pool.size()) {
        auto &bullet_at_index = pool[first_available_index];
        bullet_at_index.active = true;
        bullet_at_index.timer = 0.0f;
        bullet_at_index.position = bullet.position;
        bullet_at_index.direction = bullet.direction;
        bullet_at_index.update_function = bullet.update_function;
        first_available_index += 1;
    }
}

namespace emitter {
    enum class Type { ShootEverySecond,
                      ShootSpiral,
                      ShootNormal,
                      ShootCircular };

    static constexpr auto shoot_every_second = [](const float time, basilevs::Emitter &emitter, basilevs::World &world) {
        if (emitter.is_active) {
            if (emitter.last_shot > emitter.delay_between_shots) {

                world.enemy_bullets.add(basilevs::NormalBullet{emitter.position, Vector2{0.0, 1.0f}, basilevs::bullet::bullet_fly_forward});
                emitter.last_shot = 0.0f;
            }
            emitter.last_shot += time;
        }
    };

    static constexpr auto shoot_spiral = [](const float time, basilevs::Emitter &emitter, basilevs::World &world) {
        if (emitter.is_active) {
            if (emitter.last_shot > emitter.delay_between_shots) {
                //   for (int i = 0 ; i < 4; i++) {
                emitter.angle += 15.0f;
                if (emitter.angle > 360.0f) {
                    emitter.angle = 0.0f;
                }
                world.enemy_bullets.add(basilevs::NormalBullet{emitter.position, Vector2Rotate({0.0, 1.0f}, emitter.angle), basilevs::bullet::bullet_fly_spiral});
                //  PlaySound(emitter.sound);
                world.enemy_bullets.add(basilevs::NormalBullet{emitter.position, Vector2Rotate({0.0, 1.0f}, emitter.angle + 90), basilevs::bullet::bullet_fly_spiral});
                //  PlaySound(emitter.sound);

                world.enemy_bullets.add(basilevs::NormalBullet{emitter.position, Vector2Rotate({0.0, 1.0f}, emitter.angle + 180), basilevs::bullet::bullet_fly_spiral});
                //  PlaySound(emitter.sound);
                world.enemy_bullets.add(basilevs::NormalBullet{emitter.position, Vector2Rotate({0.0, 1.0f}, emitter.angle + 270), basilevs::bullet::bullet_fly_spiral});
                // PlaySound(emitter.sound);
                // }
                emitter.last_shot = 0.0f;
            }
            emitter.last_shot += time;
        }
    };

    static constexpr auto player_normal_shoot = [](const float time, basilevs::Emitter &emitter, basilevs::World &world) {
        if (emitter.is_active) {
            if (emitter.last_shot > emitter.delay_between_shots) {

                world.player_bullets.add(basilevs::NormalBullet{Vector2Add(emitter.position, Vector2{-5.0f, -1.0f}), Vector2{0.0f, -1.0f}, basilevs::bullet::bullet_fly_forward_fast});
                world.player_bullets.add(basilevs::NormalBullet{Vector2Add(emitter.position, Vector2{5.0f, -1.0f}), Vector2{0.0f, -1.0f}, basilevs::bullet::bullet_fly_forward_fast});
                world.player_bullets.add(basilevs::NormalBullet{emitter.position, Vector2{-1.0f, -1.0f}, basilevs::bullet::bullet_fly_forward_fast});
                world.player_bullets.add(basilevs::NormalBullet{emitter.position, Vector2{1.0f, -1.0f}, basilevs::bullet::bullet_fly_forward_fast});
                // PlaySound(emitter.sound);
                emitter.last_shot = 0.0f;
            }
        }
        emitter.last_shot += time;
    };

    static constexpr auto shoot_circular = [](const float time, basilevs::Emitter &emitter, basilevs::World &world) {
        if (emitter.is_active) {
            if (emitter.last_shot > emitter.delay_between_shots) {
                auto direction = Vector2{0.0f, 1.0f};
                for (int i = 1; i < 60; i++) {
                    //PlaySound(emitter.sound);
                    world.enemy_bullets.add(basilevs::NormalBullet{emitter.position, direction, basilevs::bullet::bullet_fly_forward});
                    direction = Vector2Rotate(direction, 360 / i);
                }
                emitter.last_shot = 0.0f;
            }
            emitter.last_shot += sin(time);
        }
    };
}// namespace emitter

void GameDefinition::initialize_world_() {

    auto player = basilevs::Player{};
    player.sprite = sprite_template_map.at(basilevs::EntityType::Player);
    player.emitter = basilevs::Emitter{{0.0f, 0.0f}, 0.023f, emitter::player_normal_shoot};
    world = basilevs::World{
            player,
            basilevs::Background("assets/basilevs_bg_001.png", 6),
            Rectangle{-10.0f, -10.0f, config::frameWidth + 10.0f, config::frameHeight + 10.0f},
            basilevs::BulletPool<basilevs::NormalBullet>(1000L), basilevs::BulletPool<basilevs::NormalBullet>(500L)};

    auto radial_emitter = basilevs::Emitter{sprite_template_map.at(basilevs::EntityType::Enemy).position, 1.5f, emitter::shoot_circular};
    auto spiral_emitter = basilevs::Emitter{sprite_template_map.at(basilevs::EntityType::Enemy).position, 0.1f, emitter::shoot_spiral};

    world.enemy_spawns = std::list<basilevs::Spawn>({
            {1.0f, Vector2{60.0, 10.0}, sprite_template_map.at(basilevs::EntityType::Enemy), basilevs::enemy::behavior_sinusoidal, radial_emitter},
            {3.0f, Vector2{60.0, 30.0}, sprite_template_map.at(basilevs::EntityType::Enemy), basilevs::enemy::behavior_sinusoidal, spiral_emitter},
    });

    world.enemies_on_screen = std::vector<basilevs::Enemy>{};
}

void GameDefinition::initialize() {
    load_textures_();
    load_sprites_();
    load_sounds_();
    initialize_world_();
    SetTargetFPS(60);
    SetTextureFilter(world.background.texture, FILTER_ANISOTROPIC_16X);
}
static void render_to_screen(raylib::RenderTexture &render_target, const basilevs::World &world) {
    DrawTexturePro(render_target.texture, Rectangle{0.0f, 0.0f, (float) render_target.texture.width, (float) -render_target.texture.height},
                   Rectangle{0.0f, 0.0f, static_cast<float>(config::screenWidth), static_cast<float>(config::screenHeight)}, Vector2{0, 0}, 0.0f, WHITE);
    raylib::DrawText(std::to_string(static_cast<int>(world.timer)), config::screenWidth - 60, 20, 30, GREEN);
    raylib::DrawText(std::to_string(world.enemy_bullets.first_available_index), config::screenWidth - 60, 60, 30, ORANGE);
    DrawFPS(5, 5);
}
void GameDefinition::loop_(std::chrono::duration<double> duration) {
    world.update(duration);
    render_();
}

void GameDefinition::run(raylib::Window &window, raylib::AudioDevice &audio) {
    std::chrono::duration<double> loop_duration = std::chrono::steady_clock::now() - std::chrono::steady_clock::now();
    while (!window.ShouldClose()) {
        auto now = std::chrono::steady_clock::now();
        loop_(loop_duration);
        loop_duration = std::chrono::steady_clock::now() - now;
    }
}
void GameDefinition::render_() {
    BeginDrawing();
    ClearBackground(BLACK);
    BeginTextureMode(render_target_);
    world.render(sprite_template_map.at(basilevs::EntityType::BulletRound));
    EndTextureMode();
    render_to_screen(render_target_, world);
    EndDrawing();
}
void basilevs::World::update(std::chrono::duration<double> duration) {
    timer += duration.count();
    background.update_background(6, GetFrameTime());
    player.update();
    spawn_enemies_();
    for (auto &enemy : enemies_on_screen) {
        enemy.behavior(enemy, timer);
        enemy.emitter.position = Vector2Add(enemy.position, enemy.emitter_offset);
        enemy.emitter.emitter_function(duration.count(), enemy.emitter, (*this));
    }
    update_bullets_(duration);
}


void basilevs::World::render(const basilevs::Sprite &bullet_sprite) {

    DrawTextureRec(background.texture, background.texture_rect, Vector2{0.0f, 0.0f}, WHITE);
    DrawTextureRec(player.sprite.texture, player.sprite.frame_rect, player.sprite.position, WHITE);
    for (auto &enemy : enemies_on_screen) {
        DrawTextureRec(enemy.animation.texture, enemy.animation.frame_rect, enemy.position, WHITE);
    }
    render_bullets_(bullet_sprite);
}

void basilevs::World::spawn_enemies_() {
    if (!enemy_spawns.empty()) {
        auto next_spawn = enemy_spawns.front();
        if (timer >= next_spawn.start_time) {
            enemies_on_screen.emplace_back(basilevs::Enemy{next_spawn.enemy, next_spawn.behavior, next_spawn.position, next_spawn.emitter});
            enemy_spawns.pop_front();
        }
    }
}

void basilevs::World::update_bullets_(const std::chrono::duration<double> &elapsed) {
    auto player_collision_rect = Rectangle{player.sprite.position.x + 14, player.sprite.position.y + 15, 5, 5};

    for (int i = 0; i < enemy_bullets.first_available_index; i++) {

        auto &bullet = enemy_bullets.pool[i];
        if (bullet.active) {
            auto bullet_collision_rect = Rectangle{bullet.position.x, bullet.position.y, 8, 8};
            if (CheckCollisionRecs(bullet_collision_rect, player_collision_rect)) {
                enemy_bullets.removeAt(i);
            }
            bullet.update_function(elapsed.count(), bullet, (*this));
            if (!CheckCollisionRecs(bullet_collision_rect, bounds)) {
                enemy_bullets.removeAt(i);
            }
        }
    }
}
void basilevs::World::render_bullets_(const basilevs::Sprite &bullet_sprite) {
    auto player_collision_rect = Rectangle{player.sprite.position.x + 14, player.sprite.position.y + 15, 5, 5};

    for (int i = 0; i < enemy_bullets.first_available_index; i++) {

        auto &bullet = enemy_bullets.pool[i];
        if (bullet.active) {
            auto bullet_collision_rect = Rectangle{bullet.position.x, bullet.position.y, 8, 8};
            if (CheckCollisionRecs(bullet_collision_rect, player_collision_rect)) {
                DrawTextureRec(bullet_sprite.texture, bullet_sprite.frame_rect, bullet.position, RED);
            } else {
                DrawTextureRec(bullet_sprite.texture, bullet_sprite.frame_rect, bullet.position, WHITE);
            }
            //raylib::DrawText(std::to_string(i), bullet.position.x-5, bullet.position.y-5, 5, ORANGE);
        }
    }
}
