//
// Created by geoco on 22.04.2021.
//

#ifndef BASILEVS_EMITTER_H
#define BASILEVS_EMITTER_H
#include <Vector2.hpp>
#include <components.h>
#include <concepts>
#include <functional>
#include <iostream>
#include <type_traits>

struct World;

class Emitter {
    using EmitterFunction = std::function<void(const double time, Emitter &emitter, World &world)>;

public:
    Emitter() = default;
    explicit Emitter(Vector2 position, float delay, EmitterFunction emitterFunction) : position{position}, delay_between_shots{delay}, emitter_function{std::move(emitterFunction)} {};
    Vector2 position{0.0f, 0.0f};
    float delay_between_shots{0.0f};
    float last_shot{0};
    bool is_active{true};
    int emission_count{0};
    float angle{0.0f};

    EmitterFunction emitter_function;

private:
    double last_emission_time_{0};
};

struct BlueprintBase {
};

template<typename T>
concept is_a_blueprint = std::is_base_of_v<BlueprintBase, T> && requires {{T::id} -> std::convertible_to<int>;};

template<is_a_component T>
auto &get(is_a_blueprint auto &emitter) {
    return std::get<T>(emitter.components);
}

struct MemoryBase {
public:
    virtual void update(double time, World &world) = 0;
    virtual ~MemoryBase()= default;
};
template<typename... Ts>
concept is_a_memory = std::is_base_of_v<MemoryBase, Ts...>;

template<is_a_component T>
auto &get(is_a_memory auto &memory) {
    return std::get<std::vector<T>>(memory.components);
}

template<is_many_components... Ts>
class EntityBlueprint : BlueprintBase {
    using ComponentFunction = std::function<void(const double, World &, Ts &...)>;
    using Components = std::tuple<Ts...>;

public:
    explicit EntityBlueprint(int id, ComponentFunction func)
        : id{id}, component_function{func} {};

    int id{0};
    ComponentFunction component_function;
    Components components = std::make_tuple(Ts()...);
};

template<is_many_components... Ts>
class BlueprintsInMemory : public MemoryBase {
    using Components = std::tuple<std::vector<Ts>...>;
    using ComponentFunction = std::function<void(const double, World &, Ts &...)>;
    using ComponentFunctionVector = std::vector<ComponentFunction>;

public:
    template<is_a_component T, is_a_blueprint... Emitters>
    static constexpr std::vector<T> unpack_components(Emitters const &...emitter_pack) { return {std::get<T>(emitter_pack.components)...}; }
    static constexpr ComponentFunction unpack_function(is_a_blueprint auto &emitter) { return emitter.component_function; }
    template<is_a_blueprint... Blueprints>
    explicit BlueprintsInMemory(Blueprints const &...blueprint_pack)
        : components{unpack_components<Ts>(blueprint_pack...)...},
          functions{unpack_function(blueprint_pack)...} {};
    Components components;
    ComponentFunctionVector functions;

    ~BlueprintsInMemory() override = default;
    void update(double time, World &world) override;
};

template<is_many_components... Ts>
void BlueprintsInMemory<Ts...>::update(double time, World &world) {
    for (int i = 0; i < functions.size(); i++) {
        functions[i](time, world, std::get<std::vector<Ts>>(components)[i]...);
    }
}
template<is_a_component... Components, is_a_blueprint... Bs>
BlueprintsInMemory(EntityBlueprint<Components...>, Bs...) -> BlueprintsInMemory<Components...>;

namespace functions {
    static constexpr auto kEmptyFunction = [](const double time, World &, components::EmitterPosition &pos, components::EmissionComponent &, components::ActiveComponent &, components::ShootComponent &) -> void {
        pos.position.x += time;
        pos.position.y += time;
        std::cout << "Time: " << time << " Position: " << pos.position.x << "," << pos.position.y << std::endl;
    };
    static constexpr auto kEmptyFunction2 = [](const double time, World &, components::EmitterPosition &pos, components::EmissionComponent &, components::ActiveComponent &, components::ShootComponent &) -> void {
        pos.direction.x += time;
        pos.direction.y += time;
        std::cout << "Time: " << time << " Direction: " << pos.direction.x << "," << pos.direction.y << std::endl;
    };
}// namespace functions
#endif//BASILEVS_EMITTER_H
