//
// Created by geoco on 21.04.2021.
//
#include <assets.h>
#include <basilevs-lib.h>
namespace basilevs {
    void Game::run(raylib::Window &window, raylib::AudioDevice &audio) {
        using namespace sml;
        using namespace state;

        GameDefinition().run(window, audio);
    }
}// namespace basilevs

void GameDefinition::initialize_world_() {
    world.background = basilevs::make_background_blueprint(textures_);
    world.player = basilevs::make_player_blueprint(textures_);
    world.enemies = basilevs::initialize_enemies(textures_);
}

void GameDefinition::initialize() {
    sm.process_event(state::events::Init{});
    textures_ = assets::load_textures_level_1();
    sounds_ = assets::load_sounds();
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
    world.player_bullets.update(duration.count(), world);
    basilevs::cleanup_bullet_pools(world);
    basilevs::play_sounds(world.sounds_queue, sounds_);

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

GameDefinition::~GameDefinition() {
    for (auto &texture : textures_) {
        UnloadTexture(texture);
    }
    for (auto &sound : sounds_) {
        UnloadSound(sound);
    }
}
