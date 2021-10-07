//
// Created by geoco on 21.04.2021.
//
#include <basilevs-lib.h>
namespace basilevs
{
    void Game::run(raylib::Window &window, raylib::AudioDevice &audio)
    {
        using namespace sml;
        using namespace state_handling;

        GameDefinition().run(window, audio);
    }
}// namespace basilevs

void GameDefinition::initialize_world_()
{
    world.background = basilevs::initialization::create_blueprint_of_background(textures_);
    world.player = basilevs::initialization::create_blueprint_of_player(textures_);
    auto level_loader = LevelLoader("assets/json/level1.json");
    world.enemies = level_loader.get_enemy_spawns(textures_);

}

void GameDefinition::initialize()
{
    sm.process_event(state_handling::events::Init{});
    textures_ = assets::load_textures_level_1();
    sounds_ = assets::load_sounds();
    initialize_world_();
    SetTargetFPS(60);
    sm.process_event(state_handling::events::Run{});
}

void GameDefinition::loop_(std::chrono::duration<double> duration)
{
    basilevs::io::handle_player_input(world);
    basilevs::game_state::update_world(duration, world);
    basilevs::collision_checking::collision_checks(world);
    basilevs::memory::cleanup_bullet_pools(world);
    basilevs::audio::play_sounds(world.sounds_queue, sounds_);

    render_();
}

void GameDefinition::run(raylib::Window &window, raylib::AudioDevice &audio)
{
    if (!state.is_initialized) {
        initialize();
    }
    std::chrono::duration<double> loop_duration = std::chrono::steady_clock::now() - std::chrono::steady_clock::now();
    while (!window.ShouldClose()) {
        auto now = std::chrono::steady_clock::now();
        loop_(loop_duration);
        loop_duration = std::chrono::steady_clock::now() - now;
    }
}

void GameDefinition::render_()
{
    BeginDrawing();
    ClearBackground(config::colors::kBackground);
    basilevs::rendering::render_to_texture(render_target_, world, textures_);
    basilevs::rendering::render_to_screen(render_target_, world);
    EndDrawing();
}

GameDefinition::~GameDefinition()
{
    for (auto &texture : textures_) {
        UnloadTexture(texture);
    }
    for (auto &sound : sounds_) {
        UnloadSound(sound);
    }
}


