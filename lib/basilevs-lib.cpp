//
// Created by geoco on 21.04.2021.
//
#include <basilevs-lib.h>

namespace basilevs {
    void Game::run() {
        using namespace sml;
        GameDefinition game;
        sm<GameState> sm{game};
        sm.process_event(Init{false});
        sm.process_event(Run{true});
        if (sm.is("running"_s)) {
            sm.process_event(Stop{});
        }
    }
}
