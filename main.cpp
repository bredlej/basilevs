#include <raylib-cpp.hpp>
#include <raylib.hpp>
#ifdef __cplusplus
extern "C" {
#endif
#include "raylib.h"
#ifdef __cplusplus
}
#endif
#include "include/basilevs-lib.h"
#include <algorithm>

int main() {
    using namespace basilevs;
    auto window = raylib::Window{config::kScreenWidth, config::kScreenHeight, "Basilevs"};
    auto audio = raylib::AudioDevice();

    auto emitter1 = TEmitter<components::Position, components::ActiveComponent, components::ShootComponent>{10};
    auto &position1 = get<components::Position>(emitter1);
    position1.position = {10.0f, 10.0f};
    auto &active1 = get<components::ActiveComponent>(emitter1);
    active1.is_active = true;
    auto emitter2 = TEmitter<components::Position, components::ActiveComponent, components::ShootComponent>{20};
    auto &position2 = get<components::Position>(emitter2);
    position2.position = {20.0f, 20.0f};
    auto &active2 = get<components::ActiveComponent>(emitter2);
    active2.is_active = false;
    auto memory = EmitterMemory<components::Position, components::ActiveComponent, components::ShootComponent>(emitter1, emitter2);
    auto position_components = get<components::Position>(memory);
    auto active_components = get<components::ActiveComponent>(memory);

  //  basilevs::Game::run(window, audio);
    return 0;
}