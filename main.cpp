#include <raylib.hpp>
#include <raylib-cpp.hpp>
#ifdef __cplusplus
extern "C" {
#endif
#include "raylib.h"
#ifdef __cplusplus
}
#endif
int main() {
    int screenWidth = 1900;
    int screenHeight = 1600;

    raylib::Window window(screenWidth, screenHeight, "raylib-cpp - basic window");

    auto target = raylib::RenderTexture2D(160, 144);
    //SetTextureFilter(target.texture, FILTER_BILINEAR);
    SetTargetFPS(60);

    while (!window.ShouldClose())
    {
        BeginDrawing();
        BeginTextureMode(target);
        window.ClearBackground(BLACK);
        DrawText("BASILEVS", 20, 20, 20, LIGHTGRAY);
        EndTextureMode();
        DrawTexturePro(target.texture, (Rectangle){ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
                       (Rectangle){ 0.0f, 0.0f, static_cast<float>(screenWidth), static_cast<float>(screenHeight) }, (Vector2){ 0, 0 }, 0.0f, WHITE);
        EndDrawing();
    }

    return 0;
}