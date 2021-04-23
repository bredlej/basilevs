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

template<is_many_components... Ts>
class EmitterMemory {
    using TupleOfVectors = std::tuple<std::vector<Ts>...>;
public:
    static constexpr std::size_t AmountComponents = sizeof...(Ts);
    TupleOfVectors emitter_tuple;

    EmitterMemory(std::initializer_list<TEmitter<Ts...>> emitters)
        : emitter_tuple {

            std::make_tuple<>(std::vector<Ts>{}...)
          } {
                  for (auto emitter: emitters) {
                      //auto [comp, idx] = {{std::get<TEmitter<Ts>>(emitter)}...};
                      std::cout << emitter.id << std::endl;
                  }
                  //auto a = {std::get<std::vector<Ts>>({emitters})...};
                  /*for (auto &emitter: emitters) {
                      auto a = decltype(emitter)();
                      //std::vector<Ts> vec = std::get<Ts...>(emitter_tuple);
                    //  vec.emplace_back(get(emitter));
                  }*/
          };

    /*void iterate(const std::initializer_list<TEmitter<Ts...>> emitters) {
        for (auto const &emit : emitters) {
            std::cout << emit.id << std::endl;
        }
    };*/
};

#endif//BASILEVS_EMITTER_H
