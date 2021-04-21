//
// Created by geoco on 21.04.2021.
//
#include <basilevs-lib.h>

namespace basilevs {
    void Game::run() {
        using namespace sml;
        GameDefinition game;
        sm<GameState> sm{game};
        sm.process_event(events::Init{});
        sm.process_event(events::Run{});
        if (sm.is("running"_s)) {
            sm.process_event(events::Stop{});
        }
    }
}
