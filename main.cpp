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
#include <boost/sml/sml.hpp>

int main() {
    using namespace basilevs;
    auto window = raylib::Window{config::kScreenWidth, config::kScreenHeight, "Basilevs"};
    auto audio = raylib::AudioDevice();

    auto emitter_func = std::function<void(const double, TWorld &, components::Movement &, components::Emission &, components::Activation &, components::Shooting &)>(functions::kEmptyFunction);
    auto emitter_func2 = std::function<void(const double, TWorld &, components::Movement &, components::Emission &, components::Activation &, components::Shooting &)>(functions::kEmptyFunction2);
    //auto emitter_func3 = std::function<void(const double, World &, components::EmitterPosition &, components::EmissionComponent &, components::ActiveComponent &, components::ShootComponent &)>(functions::kEmptyFunction2);

    auto emitter1 = Blueprint(1, emitter_func);
    auto emitter2 = Blueprint(2, emitter_func2);

    auto player = Blueprint(3, emitter_func2);
    auto memory = BlueprintsInMemory(emitter1, emitter2);
    auto w = TWorld();
    memory.update(1, w);
    memory.update(1, w);
    memory.update(10, w);
    memory.update(1, w);

    auto &position_components = get<components::Movement>(memory);
    position_components[0].position = {50.0f, 50.0f};

    auto tworld = TWorld(player, memory);
    auto tworld_enemies = tworld.enemies.get();
    tworld_enemies->update(10, w);

    struct StatefulObject {
        int i{0};
    };
    /*auto state_object = StatefulObject();
    auto state_component = components::StateMachine<basilevs::GameState, StatefulObject>(state_object);
   // state_component.state_machine.process_event(state::events::Init{});
    using namespace sml;
    using namespace state;
    std::cout << "STATE INITIALIZED: " << state_component.state_machine.is("init"_s) << std::endl;
    //basilevs::Game::run(window, audio);*/
    return 0;
}
