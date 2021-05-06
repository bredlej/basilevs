//
// Created by geoco on 22.04.2021.
//

#ifndef BASILEVS_COMPONENTS_H
#define BASILEVS_COMPONENTS_H
#include <concepts>

struct ComponentBase{};
enum class TextureId;

template<typename T>
concept is_a_component = std::is_base_of_v<ComponentBase, T>;
template<typename ...T>
concept is_many_components = (is_a_component<T> && ...);

namespace components {
    struct EmitterPosition : ComponentBase{
    public:
        Vector2 position;
        Vector2 direction;
    };

    struct SpriteComponent : ComponentBase {
        TextureId texture;
        int amount_frames{1};
        int current_frame{0};
        int frame_counter{0};
    };

    struct ShootComponent : ComponentBase {
        float delay_between_shots{0.0f};
        float last_shot{0};
        float angle{0.0f};
    };

    struct ActiveComponent : ComponentBase {
        bool is_active{true};
    };

    struct EmissionComponent : ComponentBase {
        int emission_count{0};
    };
}


#endif//BASILEVS_COMPONENTS_H
