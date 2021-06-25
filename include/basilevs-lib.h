//
// Created by geoco on 21.04.2021.
//

#ifndef BASILEVS_BASILEVS_LIB_H
#define BASILEVS_BASILEVS_LIB_H

#include <boost/sml/sml.hpp>
#include <iostream>

#include <basilevs.h>

namespace sml = boost::sml;

struct GameStateMachine{
    bool is_running{false};
    bool is_initialized{false};
};

namespace state {
    namespace events {
        struct Init {};
        struct Run {};
        struct Stop {};
    }// namespace events

    namespace guards {
        constexpr auto is_initialized = [](const auto &event, const GameStateMachine &game) { return game.is_initialized; };
        constexpr auto should_stop = [](const auto &event, const GameStateMachine &game) { return game.is_running; };
    }// namespace guards

    namespace actions {
        constexpr auto initialize = [](const auto &event, GameStateMachine &game) {
            game.is_initialized = true;
        };

        constexpr auto run_action = [](const auto &event, GameStateMachine &game) {
            game.is_running = true;
        };

        constexpr auto stop_action = [](const auto &event, GameStateMachine &game) {
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

struct GameDefinition {
public:
    TWorld world;
    GameStateMachine state;
    boost::sml::sm<basilevs::GameState> sm{state};
    void initialize();
    void run(raylib::Window &window, raylib::AudioDevice &audio);

    ~GameDefinition();
private:
    std::vector<Texture2D> textures_;
    std::vector<Sound> sounds_;
    raylib::RenderTexture2D render_target_{config::kFrameWidth, config::kFrameHeight};
    void initialize_world_();
    void loop_(std::chrono::duration<double> duration);
    void render_();
};
#endif//BASILEVS_BASILEVS_LIB_H
