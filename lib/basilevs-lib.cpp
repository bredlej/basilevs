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
    world.background = std::make_shared<TWorld::BackgroundType>(basilevs::initialization::create_background(_core.texture2d_cache));
    world.player = std::make_shared<TWorld::PlayerType>(basilevs::initialization::create_player(_core.texture2d_cache));
    auto level_loader = LevelLoader("assets/json/level1.json");
    world.enemies = level_loader.get_enemy_spawns(_core.texture2d_cache);
    world.player_bullets.first_available_index = 0;
    world.enemy_bullets.first_available_index = 0;
}

void GameDefinition::initialize()
{
    sm.process_event(state_handling::events::Init{});
    assets::load_texture_cache(_core.texture2d_cache);
    assets::load_sound_cache(_core.sound_cache);
    initialize_world_();
    SetTargetFPS(60);
    sm.process_event(state_handling::events::Run{});
}

void GameDefinition::loop_(std::chrono::duration<double> duration)
{
    handle_game_input();
    basilevs::io::handle_player_input(world);
    basilevs::game_state::update_world(duration, world);
    basilevs::collision_checking::collision_checks(world);
    basilevs::memory::cleanup_bullet_pools(world);
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
    basilevs::rendering::render_to_texture(render_target_, world, _core.texture2d_cache);
    basilevs::rendering::render_to_screen(render_target_, world);
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