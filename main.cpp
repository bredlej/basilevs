#include <raylib-cpp.hpp>
#include <raylib.hpp>
#ifdef __cplusplus
extern "C" {
#endif
#include "raylib.h"
#ifdef __cplusplus
}
#endif

#include "include/basilevs.h"
#include "include/behaviours.h"
#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <list>
int main() {

    double timer = 0;

    constexpr auto frameWidth = 160;
    constexpr auto frameHeight = 144;
    constexpr auto screenWidth = 800;
    constexpr auto screenHeight = 760;

    raylib::Window window(screenWidth, screenHeight, "Basilevs");
    auto audio = raylib::AudioDevice();
    //audio.Init();

    const auto bullet_sound = LoadSound("assets/bullet.wav");
    const auto music = LoadMusicStream("assets/music.mp3");
    SetMusicVolume(music, 0.5f);
    const auto fgColor = raylib::Color(240, 246, 240);
    const auto bgColor = raylib::Color(34, 35, 35);

    //auto basilevs_logo = LoadTextureFromImage(raylib::LoadImage("assets/basilevs.png"));

    auto render_target = raylib::RenderTexture2D(frameWidth, frameHeight);

    auto player_texture = LoadTextureFromImage(raylib::LoadImage("assets/player.png"));
    auto enemy_texture = LoadTextureFromImage(raylib::LoadImage("assets/enemy.png"));
    auto bullet_round_texture = LoadTextureFromImage(raylib::LoadImage("assets/bullet8.png"));

    auto world = basilevs::World{
            basilevs::Sprite(player_texture,
                             raylib::Vector2{64, 90}, 7),
            Rectangle{-10.0f, -10.0f, frameWidth + 10.0f, frameHeight + 10.0f},
            basilevs::BulletPool<basilevs::NormalBullet>(1000L)};


    auto enemy_sprite = basilevs::Sprite(enemy_texture, raylib::Vector2(70, 30), 1);
    auto bullet_sprite = basilevs::Sprite(bullet_round_texture, raylib::Vector2(50, 50), 1);

    auto radial_emitter = basilevs::Emitter{enemy_sprite.position, 1.5f, basilevs::emitter::shoot_circular, bullet_sound};
    auto spiral_emitter = basilevs::Emitter{enemy_sprite.position, 0.1f, basilevs::emitter::shoot_spiral, bullet_sound};
    auto background = basilevs::Background("assets/basilevs_bg_001.png", 6);

    SetTextureFilter(background.texture, FILTER_ANISOTROPIC_16X);
    SetTargetFPS(60);

    bool move_right = true;
    int movement_speed = 60;

    auto enemy_spawns = std::list<basilevs::Spawn>({
            {1.0f, Vector2{60.0, 10.0}, enemy_sprite, basilevs::enemy::behavior_sinusoidal, radial_emitter},
            {3.0f, Vector2{60.0, 30.0}, enemy_sprite, basilevs::enemy::behavior_sinusoidal, spiral_emitter},
    });

    auto enemies_on_screen = std::list<basilevs::Enemy>{};
    auto active_emitters = std::list<basilevs::SpriteEmitter>{};

    //world.enemy_bullets.add(basilevs::NormalBullet{Vector2{80.0f, 5.0f}, Vector2{0.0, 1.0f}, bullet_fly_down});

    std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - std::chrono::steady_clock::now();
    PlayMusicStream(music);
    while (!window.ShouldClose()) {
        UpdateMusicStream(music);
        //PlaySound(bullet_sound);
        auto now = std::chrono::steady_clock::now();
        if (!enemy_spawns.empty()) {
            auto next_spawn = enemy_spawns.front();
            if (timer >= next_spawn.start_time) {
                enemies_on_screen.emplace_back(basilevs::Enemy{next_spawn.enemy, next_spawn.behavior, next_spawn.position, next_spawn.emitter});
                enemy_spawns.pop_front();
            }
        }
        BeginDrawing();
        BeginTextureMode(render_target);
        window.ClearBackground(bgColor);
        DrawTextureRec(background.texture, background.texture_rect, Vector2{0.0f, 0.0f}, WHITE);
        DrawTextureRec(world.player.texture, world.player.frame_rect, world.player.position, WHITE);

        world.player.update_animation(12);
        background.update_background(12, GetFrameTime());

        if (move_right) {
            world.player.position.x += 1 * GetFrameTime() * movement_speed;
            if (world.player.position.x >= frameWidth - world.player.frame_rect.width) {
                move_right = false;
            }
        } else {
            world.player.position.x -= 1 * GetFrameTime() * movement_speed;
            if (world.player.position.x <= 0) {
                move_right = true;
            }
        }

        for (auto &enemy : enemies_on_screen) {
            DrawTextureRec(enemy.animation.texture, enemy.animation.frame_rect, enemy.position, WHITE);
            enemy.behavior(enemy, timer);
            enemy.emitter.position = Vector2Add(enemy.position, enemy.emitter_offset);
            enemy.emitter.emitter_function(elapsed.count(), enemy.emitter, world);
            //PlaySound(bullet_sound);
        }

        auto player_collision_rect = Rectangle{world.player.position.x + 14, world.player.position.y + 15, 5, 5};
        if (move_right) player_collision_rect.x -= 1;
        //DrawRectangleRec(player_collision_rect, BLUE);
        for (int i = 0; i < world.enemy_bullets.first_available_index; i++) {

            auto &bullet = world.enemy_bullets.pool[i];
            if (bullet.active) {
                auto bullet_collision_rect = Rectangle {bullet.position.x, bullet.position.y, 8, 8};
                if (CheckCollisionRecs(bullet_collision_rect, player_collision_rect)) {
                    DrawTextureRec(bullet_sprite.texture, bullet_sprite.frame_rect, bullet.position, RED);
                    world.enemy_bullets.removeAt(i);
                } else {
                    DrawTextureRec(bullet_sprite.texture, bullet_sprite.frame_rect, bullet.position, WHITE);
                }
                bullet.update_function(elapsed.count(), bullet, world);
                if (!CheckCollisionRecs(bullet_collision_rect, world.bounds)) {
                    world.enemy_bullets.removeAt(i);
                }
               // raylib::DrawText(std::to_string(i), bullet.position.x-5, bullet.position.y-5, 8, ORANGE);
            }
        }
        EndTextureMode();
        DrawTexturePro(render_target.texture, (Rectangle){0.0f, 0.0f, (float) render_target.texture.width, (float) -render_target.texture.height},
                       (Rectangle){0.0f, 0.0f, static_cast<float>(screenWidth), static_cast<float>(screenHeight)}, (Vector2){0, 0}, 0.0f, WHITE);
        raylib::DrawText(std::to_string(static_cast<int>(timer)), screenWidth - 60, 20, 30, GREEN);
        raylib::DrawText( std::to_string(world.enemy_bullets.first_available_index), screenWidth - 60, 60, 30, ORANGE);
        DrawFPS(5, 5);
        EndDrawing();

        elapsed = std::chrono::steady_clock::now() - now;

        timer += elapsed.count();
    }

    std::cout << "Done after: " << timer << " seconds " << std::endl;
    return 0;
}