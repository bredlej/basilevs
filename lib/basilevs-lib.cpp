//
// Created by geoco on 21.04.2021.
//
#include <assets.h>
#include <basilevs-lib.h>
namespace basilevs {
    void Game::run(raylib::Window &window, raylib::AudioDevice &audio) {
        using namespace sml;
        using namespace state;

        GameDefinition game;
        game.run(window, audio);
    }
}// namespace basilevs

void GameDefinition::initialize_world_() {

    world.background = basilevs::initialize_background(textures_);
    world.player = basilevs::initialize_player(textures_);
    world.enemies = basilevs::initialize_enemies(textures_);
}

void GameDefinition::initialize() {
    sm.process_event(state::events::Init{});
    textures_ = assets::load_textures_level_1();
    initialize_world_();
    SetTargetFPS(60);
    sm.process_event(state::events::Run{});
}

void GameDefinition::loop_(std::chrono::duration<double> duration) {
    basilevs::handle_player_input(world);
    world.background->update(duration.count(), world);
    world.player->update(duration.count(), world);
    world.enemies->update(duration.count(), world);
    world.enemy_bullets.update(duration.count(), world);
    basilevs::bullets_bounds_check(world);

    render_();
}

void GameDefinition::run(raylib::Window &window, raylib::AudioDevice &audio) {
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

void GameDefinition::render_() {
    BeginDrawing();
    ClearBackground(config::colors::kBackground);
    basilevs::render_to_texture(render_target_, world, textures_);
    basilevs::render_to_screen(render_target_, world);
    EndDrawing();
}
