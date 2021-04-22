//
// Created by geoco on 22.04.2021.
//

#ifndef BASILEVS_EMITTER__H
#define BASILEVS_EMITTER__H
#include "world.h"
#include <Vector2.hpp>
#include <functional>

enum class EmitterType { Down,
    Radial };

struct Emitter {
public:
    Emitter() = default;
    explicit Emitter(Vector2 position, float delay, std::function<void(const float time, Emitter &emitter, World &world)> emitterFunction) : position{position}, delay_between_shots{delay}, emitter_function{std::move(emitterFunction)} {};
    Vector2 position {0.0f, 0.0f};
    float delay_between_shots{0.0f};
    float last_shot{0};
    bool is_active{true};
    int emission_count{0};
    float angle{0.0f};

    std::function<void(const float time, Emitter &emitter, World &world)> emitter_function;

private:
    double last_emission_time_{0};
};
#endif//BASILEVS_EMITTER__H
