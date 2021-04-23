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
    auto emitter2 = TEmitter<components::Position, components::ActiveComponent, components::ShootComponent>{20};

    auto memory = EmitterMemory<components::Position, components::ActiveComponent, components::ShootComponent>({emitter1});
   /*memory.iterate({emitter1, emitter2});

    emitter1.function = [](auto d, decltype(emitter1) &emitter, auto &world) {
        auto &position = get<components::Position>(emitter);
        position.position = {1.0f, 20.0f};
    };

    auto world = World {};
    emitter1.function(1.0, emitter1, world);
*/
  //  basilevs::Game::run(window, audio);
    return 0;
}