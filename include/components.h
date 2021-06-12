//
// Created by geoco on 22.04.2021.
//

#ifndef BASILEVS_COMPONENTS_H
#define BASILEVS_COMPONENTS_H
#include "assets.h"
#include <boost/sml/sml.hpp>
#include <concepts>
#include <iostream>
struct ComponentBase{};
enum class TextureId;

template<typename T>
concept is_a_component = std::is_base_of_v<ComponentBase, T>;
template<typename ...T>
concept is_many_components = (is_a_component<T> && ...);

namespace components {
    struct Movement : ComponentBase{
    public:
        Vector2 position;
        Vector2 direction;
    };

    struct Sprite : ComponentBase {
        assets::TextureId texture{assets::TextureId::Player};
        int texture_width{1};
        int texture_height{1};
        Rectangle frame_rect{0,0,1,1};
        int amount_frames{1};
        int current_frame{0};
        int frame_counter{0};
    };

    struct Shooting : ComponentBase {
        float delay_between_shots{0.0f};
        float last_shot{0};
        float angle{0.0f};
    };

    struct Activation : ComponentBase {
        bool is_active{false};
        double activate_after_time{0.0};
    };

    struct Emission : ComponentBase {
        int emission_count{0};
    };

    struct TimeCounter : ComponentBase {
        double elapsed_time{0};
    };

    template<typename StateDeclaration, typename StatefulObject>
    struct StateMachine : ComponentBase {
        StatefulObject obj;
    public:
        explicit StateMachine(StatefulObject stateful_object) : state_machine{stateful_object} {};
        explicit StateMachine() : state_machine{obj} {};
        boost::sml::sm<StateDeclaration> state_machine;
    };
}


#endif//BASILEVS_COMPONENTS_H
