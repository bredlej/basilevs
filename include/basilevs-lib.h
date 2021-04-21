//
// Created by geoco on 21.04.2021.
//

#ifndef BASILEVS_BASILEVS_LIB_H
#define BASILEVS_BASILEVS_LIB_H

#include <boost/sml/sml.hpp>
#include <iostream>
namespace sml = boost::sml;

struct GameDefinition {
    bool is_running{false};
    bool is_initialized{false};
};

namespace events {
    struct Init {};
    struct Run {};
    struct Stop {};
}// namespace events

constexpr auto is_initialized = [](const auto &event, const GameDefinition &game) { return game.is_initialized; };

namespace actions {
    constexpr auto initialize = [](const auto &event, GameDefinition &game) {
        std::cout << "Init action" << std::endl;
        game.is_initialized = true;
    };

    constexpr auto run_action = [](const auto &event, GameDefinition &game) {
        std::cout << "Run action" << std::endl;
        game.is_running = true;
    };

    constexpr auto stop_action = [](const auto &event, GameDefinition &game) {
        std::cout << "Stop action" << std::endl;
        game.is_running = false;
    };
}// namespace actions

struct GameState {
    auto operator()() const {
        using namespace sml;
        /**
         * Initial state: *initial_state
         * Transition DSL: src_state + event [ guard ] / action = dst_state
         */
        return make_transition_table(
                *"entry"_s + event<events::Init> / actions::initialize = "init"_s,
                "init"_s + event<events::Run>[is_initialized] / actions::run_action = "running"_s,
                "running"_s + event<events::Stop> / actions::stop_action = X);
    }
};

namespace basilevs {
    struct Game {
    public:
        static void run();
    };
}// namespace basilevs
#endif//BASILEVS_BASILEVS_LIB_H
