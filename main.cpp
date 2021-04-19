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
#include <iostream>
#include <list>

void update_enemies(basilevs::World &world, const std::chrono::duration<double> &elapsed);
void handle_enemy_bullet_collisions(basilevs::World &world, basilevs::Sprite &bullet_sprite, const std::chrono::duration<double> &elapsed);
void render_to_screen(int screenWidth, int screenHeight, raylib::RenderTexture &render_target);
void render_debug_info(double timer, int screenWidth, const basilevs::World &world);

void render(double timer, int screenWidth, int screenHeight, raylib::Window &window, const raylib::Color &bgColor, raylib::RenderTexture &render_target, basilevs::Sprite &bullet_sprite, basilevs::Sprite &bullet_player_sprite, const std::chrono::duration<double> &elapsed, basilevs::World &world);

basilevs::TextureMap load_textures();
basilevs::SpriteTemplateMap load_sprites(basilevs::TextureMap &textures);
basilevs::World initialize_world(const basilevs::SpriteTemplateMap &sprite_templates) {
    const auto bullet_sound = LoadSound("assets/bullet.wav");
    auto world = basilevs::World{
            basilevs::Player{sprite_templates.at(basilevs::EntityType::Player),
                             basilevs::Emitter{{0.0f, 0.0f}, 0.023f, basilevs::emitter::player_normal_shoot, bullet_sound}},
            basilevs::Background("assets/basilevs_bg_001.png", 6),
            Rectangle{-10.0f, -10.0f, config::frameWidth + 10.0f, config::frameHeight + 10.0f},
            basilevs::BulletPool<basilevs::NormalBullet>(1000L), basilevs::BulletPool<basilevs::NormalBullet>(500L)};

    auto radial_emitter = basilevs::Emitter{sprite_templates.at(basilevs::EntityType::Enemy).position, 1.5f, basilevs::emitter::shoot_circular, bullet_sound};
    auto spiral_emitter = basilevs::Emitter{sprite_templates.at(basilevs::EntityType::Enemy).position, 0.1f, basilevs::emitter::shoot_spiral, bullet_sound};

    world.enemy_spawns = std::list<basilevs::Spawn>({
            {1.0f, Vector2{60.0, 10.0}, sprite_templates.at(basilevs::EntityType::Enemy), basilevs::enemy::behavior_sinusoidal, radial_emitter},
            {3.0f, Vector2{60.0, 30.0}, sprite_templates.at(basilevs::EntityType::Enemy), basilevs::enemy::behavior_sinusoidal, spiral_emitter},
    });

    world.enemies_on_screen = std::vector<basilevs::Enemy>{};
    return world;
}

int main() {
    using namespace basilevs;
    /*
     * The following line must be called before any textures are loaded
     */
    auto window = raylib::Window(config::screenWidth, config::screenHeight, "Basilevs");
    auto textures = load_textures();
    auto sprite_templates = load_sprites(textures);

    const auto audio = raylib::AudioDevice();
    const auto music = LoadMusicStream("assets/music.mp3");
    auto render_target = raylib::RenderTexture2D(config::frameWidth, config::frameHeight);
    auto world = initialize_world(sprite_templates);

    SetMusicVolume(music, 0.3f);
    PlayMusicStream(music);
    SetTextureFilter(world.background.texture, FILTER_ANISOTROPIC_16X);
    SetTargetFPS(60);

    std::chrono::duration<double> loop_duration = std::chrono::steady_clock::now() - std::chrono::steady_clock::now();
    double total_time_elapsed = 0;

    while (!window.ShouldClose()) {
        auto now = std::chrono::steady_clock::now();
        UpdateMusicStream(music);
        world.update(loop_duration);
        update_enemies(world, loop_duration);
        render(total_time_elapsed, config::screenWidth, config::screenHeight, window, config::colors::Background, render_target, sprite_templates.at(EntityType::BulletRound), sprite_templates.at(EntityType::BulletPlayer), loop_duration, world);

        loop_duration = std::chrono::steady_clock::now() - now;
        world.timer += loop_duration.count();
    }

    std::cout << "Done after: " << total_time_elapsed << " seconds " << std::endl;
    return 0;
}
basilevs::SpriteTemplateMap load_sprites(basilevs::TextureMap &textures) {
    basilevs::SpriteTemplateMap sprite_templates = {
            {basilevs::EntityType::Player, basilevs::Sprite(textures.at(basilevs::EntityType::Player), raylib::Vector2{64, 90}, 7)},
            {basilevs::EntityType::Enemy, basilevs::Sprite(textures.at(basilevs::EntityType::Enemy), raylib::Vector2(70, 30), 1)},
            {basilevs::EntityType::BulletRound, basilevs::Sprite(textures.at(basilevs::EntityType::BulletRound), raylib::Vector2(50, 50), 1)},
            {basilevs::EntityType::BulletPlayer, basilevs::Sprite(textures.at(basilevs::EntityType::BulletPlayer), raylib::Vector2(50, 50), 1)}};
    return sprite_templates;
}
basilevs::TextureMap load_textures() {
    basilevs::TextureMap textures = {
            {basilevs::EntityType::Player, LoadTextureFromImage(raylib::LoadImage("assets/player.png"))},
            {basilevs::EntityType::Enemy, LoadTextureFromImage(raylib::LoadImage("assets/enemy.png"))},
            {basilevs::EntityType::BulletPlayer, LoadTextureFromImage(raylib::LoadImage("assets/bullet8-002.png"))},
            {basilevs::EntityType::BulletRound, LoadTextureFromImage(raylib::LoadImage("assets/bullet8.png"))}};
    return textures;
}

void render(double timer, int screenWidth, int screenHeight, raylib::Window &window, const raylib::Color &bgColor, raylib::RenderTexture &render_target, basilevs::Sprite &bullet_sprite, basilevs::Sprite &bullet_player_sprite, const std::chrono::duration<double> &elapsed, basilevs::World &world) {
    BeginDrawing();

    BeginTextureMode(render_target);
    window.ClearBackground(bgColor);

    world.render();
    handle_enemy_bullet_collisions(world, bullet_sprite, elapsed);

    EndTextureMode();

    render_to_screen(screenWidth, screenHeight, render_target);
    render_debug_info(timer, screenWidth, world);

    EndDrawing();
}

void render_debug_info(double timer, int screenWidth, const basilevs::World &world) {
    raylib::DrawText(std::to_string(static_cast<int>(timer)), screenWidth - 60, 20, 30, GREEN);
    raylib::DrawText(std::to_string(world.enemy_bullets.first_available_index), screenWidth - 60, 60, 30, ORANGE);
    DrawFPS(5, 5);
}
void render_to_screen(int screenWidth, int screenHeight, raylib::RenderTexture &render_target) { DrawTexturePro(render_target.texture, Rectangle{0.0f, 0.0f, (float) render_target.texture.width, (float) -render_target.texture.height},
                                                                                                                Rectangle{0.0f, 0.0f, static_cast<float>(screenWidth), static_cast<float>(screenHeight)}, Vector2{0, 0}, 0.0f, WHITE); }
void handle_enemy_bullet_collisions(basilevs::World &world, basilevs::Sprite &bullet_sprite, const std::chrono::duration<double> &elapsed) {
    auto player_collision_rect = Rectangle{world.player.sprite.position.x + 14, world.player.sprite.position.y + 15, 5, 5};

    for (int i = 0; i < world.enemy_bullets.first_available_index; i++) {

        auto &bullet = world.enemy_bullets.pool[i];
        if (bullet.active) {
            auto bullet_collision_rect = Rectangle{bullet.position.x, bullet.position.y, 8, 8};
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
            //raylib::DrawText(std::to_string(i), bullet.position.x-5, bullet.position.y-5, 5, ORANGE);
        }
    }
}
void update_enemies(basilevs::World &world, const std::chrono::duration<double> &elapsed) {
    for (auto &enemy : world.enemies_on_screen) {
        enemy.behavior(enemy, world.timer);
        enemy.emitter.position = Vector2Add(enemy.position, enemy.emitter_offset);
        enemy.emitter.emitter_function(elapsed.count(), enemy.emitter, world);
    }
}

