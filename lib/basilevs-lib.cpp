//
// Created by geoco on 21.04.2021.
//
#include <basilevs-lib.h>

namespace basilevs
{
    void Game::run()
    {
        using namespace sml;
        using namespace state_handling;
        InitWindow(config::kScreenWidth, config::kScreenHeight, "Basilevs");
        InitAudioDevice();
        GameDefinition().run();
        CloseWindow();
        CloseAudioDevice();
    }
}// namespace basilevs

void GameDefinition::initialize_world_()
{
    basilevs::initialization::create_background(_core.texture2d_cache, _core.registry);
    basilevs::initialization::create_player_entt(_core.texture2d_cache, _core.registry);
    _core.registry.ctx().emplace<components::Frame>(config::kFrameBoundLeft, config::kFrameBoundUp, 260, 260);
    auto level_loader = LevelLoader("assets/json/level1.json");
    level_loader.load_enemy_spawns(_core.texture2d_cache, _core.registry);
}

void GameDefinition::initialize()
{
    sm.process_event(state_handling::events::Init{});
    assets::load_texture_cache(_core.texture2d_cache);
    assets::load_sound_cache(_core.sound_cache);
    _core.registry.ctx().emplace<input::UserInput<input::PlayerInput>>();
    initialize_world_();
    SetTargetFPS(120);
    sm.process_event(state_handling::events::Run{});
}

void GameDefinition::loop_(std::chrono::duration<double> duration)
{
    handle_game_input();
    basilevs::io::handle_player_input(_core);
    basilevs::game_state::update(duration, _core.registry);
    basilevs::collision_checking::collision_checks(_core.registry);
    basilevs::memory::cleanup_bullet_pools(_core.registry);
    basilevs::audio::play_sounds(world.sounds_queue, _core.sound_cache);

    render_();
}

void GameDefinition::run()
{
    if (!state.is_initialized) { initialize(); }
    std::chrono::duration<double> loop_duration = std::chrono::steady_clock::now() - std::chrono::steady_clock::now();
    while (!WindowShouldClose()) {
        auto now = std::chrono::steady_clock::now();
        loop_(loop_duration);
        loop_duration = std::chrono::steady_clock::now() - now;
    }
}

void GameDefinition::render_()
{
    BeginDrawing();
    ClearBackground(config::colors::kBackground);
    basilevs::rendering::render_to_texture(render_target_, _core);
    basilevs::rendering::render_to_screen(render_target_, _core);
    EndDrawing();
}

GameDefinition::~GameDefinition()
{
    // unload all textures from _core.texture_cache
    for (const auto &texture : _core.texture2d_cache | std::views::values) { UnloadTexture(texture); }
    for (const auto &sound: _core.sound_cache | std::views::values) { UnloadSound(sound); }
}

void GameDefinition::handle_game_input()
{
    register_input({KEY_F10}, input::GameInput::Restart, game_input);

    // TODO handle this with state machine
    if (game_input[input::GameInput::Restart]) { initialize_world_(); }
}