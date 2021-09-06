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

    /*
     * Allows an object to be moved from its current position towards a direction with a given speed.
     */
    struct Movement : ComponentBase{
    public:
        raylib::Vector2 position;
        raylib::Vector2 direction;
        float speed{0.0f};
    };

    /*
     * Defines how to render the object on screen.
     */
    struct Sprite : ComponentBase {
        /*
         * Id of the texture representing this object. Must correspond to a specific texture declared in assets.h
         */
        assets::TextureId texture{assets::TextureId::Player};
        int texture_width_px{1};
        int texture_height_px{1};
        float rotation_degrees{0.0};
        /*
         * Defines a rectangular area representing the part of a texture that will be rendered on screen.
         */
        Rectangle frame_rect{0,0,1,1};
        /*
         * Defines how much single frames the animation of this object consists of.
         */
        uint32_t amount_frames{1};
        /*
         * Defines which animation frame is currently active. Indexed from 0 to amount_frames - 1
         */
        uint32_t current_visible_frame{0};
        /*
         * Used for counting since how much clock ticks the current frame is visible
         */
        uint32_t fps_counter{0};
        /*
         * Defines the speed with which the animation changes its frame per seconds. Higher values result in faster animations.
         */
        float fps_speed{1.0f};
        /*
         * Declares the offset of this sprite relative to the 0,0 position (top-left). Used for example when bullets are shot by enemies,
         * to declare their initial position in an exact place relative to the enemy's sprite.
         * The actual position where the sprite is rendered is usually defined in the Movement components `position` value, so this is more of a helper attribute.
         * TODO check if this is a necessary sprite attribute or can be refactored somehow
         */
        raylib::Vector2 offset{0.0f, 0.0f};
        /*
         * Size of the sprite represented by a distance from 0,0. Used for bound checking if sprite is outside the frame by a given margin.
         * TODO maybe remove this and use Collision component instead
         */
        raylib::Vector2 bounds{8, 8};
    };

    struct Collision : ComponentBase {
        struct {
            raylib::Vector2 center{0.0f, 0.0f};
            float radius{0.0f};
        } bounds;

        bool is_collidable{false};
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

    template<typename StateDeclaration, typename StatefulObject>
    struct StateMachine : ComponentBase {
        StatefulObject obj;
    public:
        explicit StateMachine(StatefulObject stateful_object) : state_machine{stateful_object} {};
        explicit StateMachine() : state_machine{obj} {};
        boost::sml::sm<StateDeclaration> state_machine;
    };

    struct Damage : ComponentBase {
        float value{1.0f};
    };

    struct Health : ComponentBase {
        double hp{100.0};
    };
}


#endif//BASILEVS_COMPONENTS_H
