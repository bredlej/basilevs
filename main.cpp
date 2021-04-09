#include <raylib.hpp>
#include <raylib-cpp.hpp>
#ifdef __cplusplus
extern "C" {
#endif
#include "raylib.h"
#ifdef __cplusplus
}
#endif

#include "include/core.h"

int main() {

    int frameWidth = 160;
    int frameHeight = 144;
    int screenWidth = 800;
    int screenHeight = 760;

    auto fgColor = raylib::Color(240, 246, 240);
    auto bgColor = raylib::Color(34, 35, 35);

    raylib::Window window(screenWidth, screenHeight, "Basilevs");

    auto basilevs_logo = LoadTextureFromImage(raylib::LoadImage("assets/basilevs.png"));

    auto target = raylib::RenderTexture2D(160, 144);
    auto player = core::PlayerDefinition("assets/player.png", raylib::Vector2{64, 90}, 7);
    auto background = core::Background("assets/basilevs_bg_001.png", 6);
    SetTextureFilter(background.texture, FILTER_ANISOTROPIC_16X);
    SetTargetFPS(60);

    bool move_right = true;

    int movement_speed = 60;

    while (!window.ShouldClose())
    {
        BeginDrawing();
        BeginTextureMode(target);
        window.ClearBackground(bgColor);
        //DrawTexture(player.texture, static_cast<int>(player.position.x), static_cast<int>(player.position.y), WHITE);
        DrawTextureRec(background.texture, background.texture_rect, Vector2{0.0f, 0.0f}, WHITE);
        DrawTextureRec(player.texture, player.frame_rect, player.position, WHITE);
        player.update_animation(12);
        background.update_background(12, GetFrameTime());

        if (move_right) {
            player.position.x += 1 * GetFrameTime() * movement_speed;
            if (player.position.x >= frameWidth - player.frame_rect.width) {
                move_right = false;
            }
        }
        else {
            player.position.x -= 1 * GetFrameTime() * movement_speed;
            if (player.position.x <= 0) {
                move_right = true;
            }
        }

        EndTextureMode();
        DrawTexturePro(target.texture, (Rectangle){ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
                       (Rectangle){ 0.0f, 0.0f, static_cast<float>(screenWidth), static_cast<float>(screenHeight) }, (Vector2){ 0, 0 }, 0.0f, WHITE);
        EndDrawing();
    }

    return 0;
}