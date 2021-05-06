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

    auto emitter_func = std::function<void(const double, World &, components::EmitterPosition &, components::EmissionComponent &, components::ActiveComponent &, components::ShootComponent &)>(functions::kEmptyFunction);
    auto emitter_func2 = std::function<void(const double, World &, components::EmitterPosition &, components::EmissionComponent &, components::ActiveComponent &, components::ShootComponent &)>(functions::kEmptyFunction2);

    auto emitter1 = EntityBlueprint(1, emitter_func);
    auto emitter2 = EntityBlueprint(2, emitter_func2);


    auto memory = BlueprintsInMemory(emitter1, emitter2);
    auto w = World();
    memory.update(1, w);
    memory.update(1, w);
    memory.update(10, w);
    memory.update(1, w);

    auto &position_components = get<components::EmitterPosition>(memory);
    position_components[0].position = {50.0f, 50.0f};

    auto tworld = TWorld(memory);
    auto tworld_enemies = tworld.enemies.get();
    tworld_enemies->update(10, w);

    //basilevs::Game::run(window, audio);
    return 0;
}