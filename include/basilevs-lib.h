//
// Created by geoco on 21.04.2021.
//

#ifndef BASILEVS_BASILEVS_LIB_H
#define BASILEVS_BASILEVS_LIB_H

#include <behaviours.h>
#include <boost/sml/sml.hpp>
#include <iostream>
#include <raylib-cpp/include/Functions.hpp>
#include <raylib-cpp/include/Vector2.hpp>
#include <raylib.h>
#include <world.h>

namespace sml = boost::sml;

struct GameDefinition {
public:
    World world;
    bool is_running{false};
    bool is_initialized{false};

    void initialize();
    void run(raylib::Window &window, raylib::AudioDevice &audio);

private:
    mutable basilevs::TextureMap texture_map;
    mutable basilevs::SpriteTemplateMap sprite_template_map;
    mutable basilevs::SoundMap sound_map;

    raylib::RenderTexture2D render_target_{config::kFrameWidth, config::kFrameHeight};
    void load_textures_() const;
    void load_sprites_() const;
    void load_sounds_() const;
    void initialize_world_();
    void loop_(std::chrono::duration<double> duration);
    void render_();
};

namespace state {
    namespace events {
        struct Init {};
        struct Run {
            raylib::Window &window;
            raylib::AudioDevice &audio;
        };
        struct Stop {};
    }// namespace events

    namespace guards {
        constexpr auto is_initialized = [](const auto &event, const GameDefinition &game) { return game.is_initialized; };
        constexpr auto should_stop = [](const auto &event, const GameDefinition &game) { return game.is_running; };
    }// namespace guards

    namespace actions {
        constexpr auto initialize = [](const auto &event, GameDefinition &game) {
            game.initialize();
            game.is_initialized = true;
        };

        constexpr auto run_action = [](const auto &event, GameDefinition &game) {
            game.run(event.window, event.audio);
            game.is_running = true;
        };

        constexpr auto stop_action = [](const auto &event, GameDefinition &game) {
            game.is_running = false;
        };
    }// namespace actions
}// namespace state

namespace basilevs {
    struct GameState {
        auto operator()() const {
            using namespace sml;
            using namespace state;
            return make_transition_table(
                    *"entry"_s + event<events::Init> / actions::initialize = "init"_s,
                    "init"_s + event<events::Run>[guards::is_initialized] / actions::run_action = "running"_s,
                    "running"_s + event<events::Stop>[guards::should_stop] / actions::stop_action = X);
        }
    };

    struct Game {
    public:
        static void run(raylib::Window &window, raylib::AudioDevice &audio);
    };
}// namespace basilevs
#endif//BASILEVS_BASILEVS_LIB_H
