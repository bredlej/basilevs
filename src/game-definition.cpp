//
// Created by geoco on 21.04.2021.
//
#include "../include/pool.hpp"
#include "rlimgui/rlImGui.h"

#include <game-definition.h>

namespace basilevs
{
    void Game::run()
    {
        using namespace sml;
        using namespace state_handling;
        InitWindow(config::kScreenWidth, config::kScreenHeight, "Basilevs");
        InitAudioDevice();
        rlImGuiSetup(true);
        GameDefinition().run();
        rlImGuiShutdown();
        CloseWindow();
        CloseAudioDevice();
    }
}// namespace basilevs

template<typename T>
entt::entity create_initial_bullet(entt::registry &registry)
{
    return BlueprintEntt(registry).builder()
                                  .with<TWorld::BulletStateComponent>()
                                  .with<components::UpdateFunction>()
                                  .with<components::Sprite>()
                                  .with<components::Movement>()
                                  .with<components::Collision>()
                                  .with<components::TimeCounter>()
                                  .with<components::Damage>()
                                  .with<T>()
                                  .build();
}

template<typename T, int N>
std::vector<entt::entity> create_bullet_vector(entt::registry &registry)
{
    std::vector<entt::entity> bullets(N);
    bullets.reserve(N);
    std::ranges::generate_n(bullets.begin(), N, [&registry]() { return create_initial_bullet<T>(registry); });
    return bullets;
}

void GameDefinition::_initialize_world()
{
    basilevs::initialization::create_background(_core.texture2d_cache, _core.registry);
    basilevs::initialization::create_player_entt(_core.texture2d_cache, _core.registry);
    _core.registry.ctx().emplace<components::PlayerBullets>(create_bullet_vector<components::PlayerBullet, 1000>(_core.registry), std::vector<entt::entity>{});
    _core.registry.ctx().emplace<components::EnemyBullets>(create_bullet_vector<components::EnemyBullet, 1000>(_core.registry), std::vector<entt::entity>{});
    _core.registry.ctx().emplace<components::Frame>(config::kFrameBoundLeft, config::kFrameBoundUp, 260, 260);
    _core.registry.ctx().emplace<components::SpatialGrid>(config::kFrameWidth, config::kFrameHeight, config::kCellSize);
    auto level_loader = LevelLoader("assets/json/level1.json");
    level_loader.load_enemy_spawns(_core.texture2d_cache, _core.registry);
}

void GameDefinition::initialize()
{
    sm.process_event(state_handling::events::Init{});
    assets::load_texture_cache(_core.texture2d_cache);
    assets::load_sound_cache(_core.sound_cache);
    _core.registry.ctx().emplace<input::UserInput<input::PlayerInput>>();
    _initialize_world();

    _core.registry.ctx().emplace<debug::RealTimeToggle>(true);
    _core.registry.ctx().emplace<debug::ShowDebugGrid>(true);

    SetTargetFPS(120);
    sm.process_event(state_handling::events::Run{});
}

void GameDefinition::_loop(const std::chrono::duration<double> duration)
{
    _handle_game_input();
    basilevs::io::handle_player_input(_core);
    if (_core.registry.ctx().get<debug::RealTimeToggle>().enabled) { basilevs::game_state::update(duration, _core.registry); } else {
        if (_core.registry.ctx().contains<debug::AdvanceFrame>()) {
            const auto fixed_duration = _core.registry.ctx().get<debug::AdvanceFrame>().duration;
            basilevs::game_state::update(fixed_duration, _core.registry);
            _core.registry.ctx().erase<debug::AdvanceFrame>();
        }
    }
    basilevs::collision_checking::collision_checks(_core.registry);
    basilevs::memory::cleanup_bullet_pools(_core.registry);
    basilevs::audio::play_sounds(world.sounds_queue, _core.sound_cache);

    _render();
}

void GameDefinition::run()
{
    if (!state.is_initialized) { initialize(); }
    std::chrono::duration<double> loop_duration = std::chrono::steady_clock::now() - std::chrono::steady_clock::now();
    while (!WindowShouldClose()) {
        auto now = std::chrono::steady_clock::now();
        _loop(loop_duration);
        loop_duration = std::chrono::steady_clock::now() - now;
    }
}

void GameDefinition::_render()
{
    BeginDrawing();
    ClearBackground(config::colors::kBackground);
    basilevs::rendering::render_to_texture(render_target_, _core);
    basilevs::rendering::render_to_screen(render_target_, _core);
    _render_debug();

    EndDrawing();
}

inline void render_grid(const entt::registry &registry)
{
    constexpr auto frame_to_screen_width_ratio = static_cast<float>(config::kScreenWidth) / static_cast<float>(config::kFrameWidth);
    constexpr auto frame_to_screen_height_ratio = static_cast<float>(config::kScreenHeight) / static_cast<float>(config::kFrameHeight);

    const components::SpatialGrid &spatial_grid = registry.ctx().get<components::SpatialGrid>();
    constexpr auto cell_size = config::kCellSize;
    for (int i = 0; i < config::kScreenWidth; i += cell_size * frame_to_screen_width_ratio) {
        DrawLine(i, 0, i, config::kScreenHeight, RED);
    }
    for (int i = 0; i < config::kScreenHeight; i += cell_size * frame_to_screen_height_ratio) {
        DrawLine(0, i, config::kScreenWidth, i, RED);
    }

    for (int y = 0; y < config::kFrameHeight; y += cell_size) {
        for (int x = 0; x < config::kFrameWidth; x += cell_size) {
            const auto cell = spatial_grid.query(x, y);
            auto enemy_bullets = 0;
            auto player_bullets = 0;
            auto enemies = 0;
            auto player = 0;
            for (const auto entity : cell) {
                if (registry.any_of<components::PlayerBullet>(entity)) { player_bullets++; }
                if (registry.any_of<components::EnemyBullet>(entity)) { enemy_bullets++; }
                if (registry.any_of<TWorld::EnemyStateComponent>(entity)) { enemies++; }
                if (registry.any_of<components::Player>(entity)) { player++; }
            }
            DrawText(TextFormat("%d", player_bullets), (x * frame_to_screen_width_ratio) + 5, y * frame_to_screen_height_ratio + 5, 10, YELLOW);
            DrawText(TextFormat("%d", enemy_bullets), (x * frame_to_screen_width_ratio) + 5, y * frame_to_screen_height_ratio + 15, 10, RED);
            DrawText(TextFormat("%d", enemies), (x * frame_to_screen_width_ratio) + 5, y * frame_to_screen_height_ratio + 25, 10, BLUE);
            DrawText(TextFormat("%d", player), (x * frame_to_screen_width_ratio) + 5, y * frame_to_screen_height_ratio + 35, 10, WHITE);

        }
    }

}

void GameDefinition::_render_debug()
{
    auto &debug_grid = _core.registry.ctx().get<debug::ShowDebugGrid>().enabled;

    if (debug_grid) {
        render_grid(_core.registry);
    }
    rlImGuiBegin();
    ImGui::Begin("Basilevs Debug");
    auto &realtime_toggle = _core.registry.ctx().get<debug::RealTimeToggle>().enabled;
    ImGui::Checkbox("Real time", &realtime_toggle);
    if (!realtime_toggle) {
        ImGui::SameLine();
        if (ImGui::Button("Next frame")) { _core.registry.ctx().emplace<debug::AdvanceFrame>(std::chrono::duration<double>(1.0 / 120.0)); }
    }
    ImGui::Checkbox("Show Grid", &debug_grid);
    auto &[free_enemy_bullets, active_enemy_bullets] = _core.registry.ctx().get<components::EnemyBullets>();
    ImGui::Text("Enemy bullets active: %d", active_enemy_bullets.size());
    ImGui::Text("Enemy bullets free: %d", free_enemy_bullets.size());

    auto &[free_player_bullets, active_player_bullets] = _core.registry.ctx().get<components::PlayerBullets>();
    ImGui::Text("Player bullets active: %d", active_player_bullets.size());
    ImGui::Text("Player bullets free: %d", free_player_bullets.size());

    for (const entt::entity player_bullet : active_player_bullets) {
        const auto &movement = _core.registry.get<components::Movement>(player_bullet);
        const auto &sprite = _core.registry.get<components::Sprite>(player_bullet);
        ImGui::Text("Player bullet %d: %f, %f", player_bullet, movement.position.x, movement.position.y);
    }


    ImGui::End();
    rlImGuiEnd();
}


GameDefinition::~GameDefinition()
{
    // unload all textures from _core.texture_cache
    for (const auto &texture : _core.texture2d_cache | std::views::values) { UnloadTexture(texture); }
    for (const auto &sound : _core.sound_cache | std::views::values) { UnloadSound(sound); }
}

void GameDefinition::_handle_game_input()
{
    register_input({KEY_F10}, input::GameInput::Restart, game_input);

    // TODO handle this with state machine
    if (game_input[input::GameInput::Restart]) { _initialize_world(); }
}