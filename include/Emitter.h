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

struct EmitterBase {
};
template<typename T>
concept is_an_emitter = std::is_base_of_v<EmitterBase, T>;

template<is_many_components... Ts>
class TEmitter : EmitterBase {
    using EmitterFunction = std::function<void(const double time, TEmitter<Ts...> &emitter, World &world)>;
    using Components = std::tuple<Ts...>;

public:
    explicit TEmitter(int id) : id{id} {};
    int id{0};
    EmitterFunction function;
    Components components = std::make_tuple(Ts()...);
};

template<is_a_component T>
auto &get(is_an_emitter auto &emitter) {
    return std::get<T>(emitter.components);
}

struct EmitterMemoryBase {};
template<typename... Ts>
concept is_a_memory = std::is_base_of_v<EmitterMemoryBase, Ts...>;

template<is_a_component T>
auto &get(is_a_memory auto &memory) {
    return std::get<std::vector<T>>(memory.components);
}

template<is_many_components... Ts>
class EmitterMemory : EmitterMemoryBase {
    using TupleOfVectors = std::tuple<std::vector<Ts>...>;
public:
    static constexpr std::size_t AmountComponents = sizeof...(Ts);
    template <is_a_component T, is_an_emitter ... Emitters>
    static constexpr std::vector<T> unpack_components(Emitters const & ...emitter_pack) { return { get<T>(emitter_pack)... }; }
    TupleOfVectors components;

    explicit EmitterMemory(is_an_emitter auto &...emitter_pack)
            : components { unpack_components<Ts>(emitter_pack...)... }
    { };
};

#endif//BASILEVS_EMITTER_H
