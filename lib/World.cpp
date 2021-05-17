//
// Created by geoco on 22.04.2021.
//
#include "world.h"
/*
void World::update(std::chrono::duration<double> duration) {
    timer += duration.count();
    background.update_background(6, GetFrameTime());
    player.update();
    spawn_enemies_();
    update_emitters_(duration);
    update_bullets_(duration);
}

void World::render(const Sprite &bullet_sprite) {

    DrawTextureRec(background.texture, background.texture_rect, Vector2{0.0f, 0.0f}, WHITE);
    DrawTextureRec(player.sprite.texture, player.sprite.frame_rect, player.sprite.position, WHITE);
    for (auto &enemy : enemies_on_screen) {
        DrawTextureRec(enemy.animation.texture, enemy.animation.frame_rect, enemy.position, WHITE);
    }
    render_bullets_(bullet_sprite);
}

void World::spawn_enemies_() {
    if (!enemy_spawns.empty()) {
        auto next_spawn = enemy_spawns.front();
        if (timer >= next_spawn.start_time) {
            enemies_on_screen.emplace_back(Enemy{next_spawn.enemy, next_spawn.behavior, next_spawn.position, next_spawn.emitter});
            enemy_spawns.pop_front();
        }
    }
}

void World::update_bullets_(const std::chrono::duration<double> &elapsed) {
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
void World::render_bullets_(const Sprite &bullet_sprite) {
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

void World::update_emitters_(const std::chrono::duration<double> &elapsed) {
    for (auto &enemy : enemies_on_screen) {
        enemy.behavior(enemy, timer);
        enemy.emitter.position = Vector2Add(enemy.position, enemy.emitter_offset);
        enemy.emitter.emitter_function(elapsed.count(), enemy.emitter, (*this));
    }
}
 */