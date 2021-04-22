//
// Created by geoco on 22.04.2021.
//

#ifndef BASILEVS_EMITTER_H
#define BASILEVS_EMITTER_H
#include <Vector2.hpp>
#include <functional>

struct World;
class Emitter {
    using EmitterFunction = std::function<void(const double time, Emitter &emitter, World &world)>;
public:
    Emitter() = default;
    explicit Emitter(Vector2 position, float delay, EmitterFunction emitterFunction) : position{position}, delay_between_shots{delay}, emitter_function{std::move(emitterFunction)} {};
    Vector2 position {0.0f, 0.0f};
    float delay_between_shots{0.0f};
    float last_shot{0};
    bool is_active{true};
    int emission_count{0};
    float angle{0.0f};

    EmitterFunction emitter_function;

private:
    double last_emission_time_{0};
};


#endif//BASILEVS_EMITTER_H