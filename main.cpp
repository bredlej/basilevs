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

    auto emitter_func = std::function<void(const double, World &, components::Position &, components::EmissionComponent &, components::ActiveComponent &, components::ShootComponent &)>(functions::kEmptyFunction);

    auto emitter1 = TEmitter(1, emitter_func);
    auto emitter2 = TEmitter(2, emitter_func);

    auto memory = EmitterMemory(emitter1, emitter2);

    auto &position_components = get<components::Position>(memory);
    position_components[0].position = {50.0f, 50.0f};


  //  basilevs::Game::run(window, audio);
    return 0;
}