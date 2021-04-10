#include <raylib-cpp.hpp>
#include <raylib.hpp>
#ifdef __cplusplus
extern "C" {
#endif
#include "raylib.h"
#ifdef __cplusplus
}
#endif

#include "include/core.h"
#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <list>
int main() {

    double timer = 0;

    int frameWidth = 160;
    int frameHeight = 144;
    int screenWidth = 800;
    int screenHeight = 760;

    raylib::Window window(screenWidth, screenHeight, "Basilevs");

    auto fgColor = raylib::Color(240, 246, 240);
    auto bgColor = raylib::Color(34, 35, 35);

    //auto basilevs_logo = LoadTextureFromImage(raylib::LoadImage("assets/basilevs.png"));

    auto render_target = raylib::RenderTexture2D(160, 144);

    auto player_texture = LoadTextureFromImage(raylib::LoadImage("assets/player.png"));
    auto player_sprite = core::Sprite(player_texture, raylib::Vector2{64, 90}, 7);

    auto enemy_texture = LoadTextureFromImage(raylib::LoadImage("assets/enemy.png"));
    auto enemy_sprite = core::Sprite(enemy_texture, raylib::Vector2(70, 30), 1);

    auto bullet8_texture = LoadTextureFromImage(raylib::LoadImage("assets/bullet8.png"));
    auto bullet_sprite = core::Sprite(bullet8_texture, raylib::Vector2(50, 50), 1);
    auto background = core::Background("assets/basilevs_bg_001.png", 6);

    SetTextureFilter(background.texture, FILTER_ANISOTROPIC_16X);
    SetTargetFPS(60);

    bool move_right = true;

    int movement_speed = 60;

    auto clock = std::chrono::steady_clock{};

    struct Spawn {
        float start_time;
        Vector2 position;
        core::Sprite enemy;
        std::function<void(core::Enemy &, float)> behavior;
    };

    auto behavior_sinusoidal = [&](core::Enemy &enemy, double time) -> void {
        enemy.position.x += sin(time) * 0.7;
    };

    auto enemy_spawns = std::list<Spawn>({{5.0f, Vector2{60.0, 10.0}, enemy_sprite, behavior_sinusoidal},
                                          {8.0f, Vector2{60.0, 60.0}, bullet_sprite, behavior_sinusoidal}});

    auto enemies_on_screen = std::list<core::Enemy>{};

    while (!window.ShouldClose()) {
        auto now = std::chrono::steady_clock::now();
        if (!enemy_spawns.empty()) {
            auto next_spawn = enemy_spawns.front();
            if (timer >= next_spawn.start_time) {
                enemies_on_screen.emplace_back(core::Enemy{next_spawn.enemy, behavior_sinusoidal, next_spawn.position});
                enemy_spawns.pop_front();
            }
        }
        BeginDrawing();
        BeginTextureMode(render_target);
        window.ClearBackground(bgColor);
        DrawTextureRec(background.texture, background.texture_rect, Vector2{0.0f, 0.0f}, WHITE);
        DrawTextureRec(player_sprite.texture, player_sprite.frame_rect, player_sprite.position, WHITE);

        player_sprite.update_animation(12);
        background.update_background(12, GetFrameTime());

        if (move_right) {
            player_sprite.position.x += 1 * GetFrameTime() * movement_speed;
            if (player_sprite.position.x >= frameWidth - player_sprite.frame_rect.width) {
                move_right = false;
            }
        } else {
            player_sprite.position.x -= 1 * GetFrameTime() * movement_speed;
            if (player_sprite.position.x <= 0) {
                move_right = true;
            }
        }

        for (auto &enemy : enemies_on_screen) {
            DrawTextureRec(enemy.animation.texture, enemy.animation.frame_rect, enemy.position, WHITE);
            enemy.behavior(enemy, timer);
        }
        EndTextureMode();
        DrawTexturePro(render_target.texture, (Rectangle){0.0f, 0.0f, (float) render_target.texture.width, (float) -render_target.texture.height},
                       (Rectangle){0.0f, 0.0f, static_cast<float>(screenWidth), static_cast<float>(screenHeight)}, (Vector2){0, 0}, 0.0f, WHITE);
        raylib::DrawText(std::to_string(static_cast<int>(timer)), screenWidth - 60, 20, 30, GREEN);
        EndDrawing();
        std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - now;

        timer += elapsed.count();
    }

    std::cout << "Done after: " << timer << " seconds " << std::endl;
    return 0;
}