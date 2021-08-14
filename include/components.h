//
// Created by geoco on 22.04.2021.
//

#ifndef BASILEVS_COMPONENTS_H
#define BASILEVS_COMPONENTS_H
#include "assets.h"
#include <boost/sml/sml.hpp>
#include <concepts>
#include <functional>
#include <iostream>
struct ComponentBase{};
struct TWorld;

enum class TextureId;

template<typename T>
concept is_a_component = std::is_base_of_v<ComponentBase, T>;
template<typename ...T>
concept is_many_components = (is_a_component<T> && ...);

namespace components {
    struct Movement : ComponentBase{
    public:
        raylib::Vector2 position;
        raylib::Vector2 direction;
        float speed{0.0f};
    };

    struct Sprite : ComponentBase {
        assets::TextureId texture{assets::TextureId::Player};
        int texture_width{1};
        int texture_height{1};
        float rotation{0.0};
        Rectangle frame_rect{0,0,1,1};
        uint32_t amount_frames{1};
        uint32_t current_frame{0};
        uint32_t frame_counter{0};
        raylib::Vector2 offset{0.0f, 0.0f};
        raylib::Vector2 bounds{8, 8};
        float frame_speed{1.0f};
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
        uint32_t emission_count{0};
        double last_emission{0.0};
    };

    struct TimeCounter : ComponentBase {
        double elapsed_time{0.0};
    };

    struct CollisionCheck : ComponentBase {
        std::function<bool(TWorld&)> is_collision;
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
