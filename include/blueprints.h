//
// Created by geoco on 22.04.2021.
//

#ifndef BASILEVS_BLUEPRINTS_H
#define BASILEVS_BLUEPRINTS_H

#include <Vector2.hpp>
#include <components.h>
#include <concepts>
#include <functional>
#include <iostream>
#include <type_traits>

/* Forward declaration */
struct TWorld;

struct BlueprintBase {
    virtual void update(double time, TWorld &world) = 0;
    virtual ~BlueprintBase() = default;
};

template<typename T>
concept is_a_blueprint = std::is_base_of_v<BlueprintBase, T>;

/**
 * Helper function to get the Component of type T from a Blueprint
 * @tparam T type specified by the is_a_component concept (see components.h)
 * @param blueprint object to get the component from.
 * @return reference to the component if present in the Blueprint.
 */
template<is_a_component T>
auto &get(is_a_blueprint auto &blueprint) {
    return std::get<T>(blueprint.components);
}

struct MemoryBase {
public:
    virtual void update(double time, TWorld &world) = 0;
    virtual ~MemoryBase() = default;
};

template<typename T>
concept is_a_memory = std::is_base_of_v<MemoryBase, T>;

template<is_a_component T>
auto &get(is_a_memory auto &memory) {
    return std::get<std::vector<T>>(memory.components);
}
/*
 * Describes a structure defined by a set of components and a function which acts upon them and provides a reference to a TWorld object.
 * Components are used to describe the properties and behaviour of a Blueprint.
 * For example a blueprint defined as:
 *
 *      Blueprint<Position, Direction, Rotation>

 * will consist of:
 *
 *      - std::tuple<>
 *        |- Position()
 *        |- Direction()
 *        |- Rotation()
 *      - std::function<void(const double, TWorld &, Position &, Direction &, Rotation &)>
 *
 * The `const double` argument is intended to provide an amount of time, with which the component is to be updated, and `TWorld &` allows accessing other game objects.
 *
 * A given component can be accessed with use of the `auto &get(is_a_blueprint auto &blueprint)` function defined above.
 * Example:
 *
 *       auto &position_component = get<Position>(blueprint);
 *            ^
 *            |
 *            use a reference if you want to change the components internal values
 *
 * @tparam Ts a list of components which adhere to the is_many_components concept (see components.h).
 */
template<is_many_components... Ts>
class Blueprint : public BlueprintBase {
    using BlueprintUpdateFunction = std::function<void(const double, TWorld &, Ts &...)>;
    using Components = std::tuple<Ts...>;

public:
    BlueprintUpdateFunction update_function;
    Components components = std::make_tuple(Ts()...);

    explicit Blueprint(BlueprintUpdateFunction blueprint_update_function)
        : update_function{blueprint_update_function} {};
    void update(double time, TWorld &world) override;
    ~Blueprint() override = default;
};

template<is_many_components... Ts>
void Blueprint<Ts...>::update(double time, TWorld &world) {
    update_function(time, world, std::get<Ts>(components)...);
}

/**
 * Deduction guide allowing to infer Blueprint template types from the function provided in the constructor
 *
 * @tparam Cs set of components adhering to is_a_component concept (see components.h)
 */
template<is_a_component... Cs>
Blueprint(std::function<void(const double, TWorld &, Cs &...)>) -> Blueprint<Cs...>;

/*
 * BlueprintsInMemory is a utility class which given a collection of Blueprint objects, takes them apart and stores their components and functions in vectors.
 * The class stores copies of the original objects, so it's best to discard them after BlueprintsInMemory is created.
 * As an example how it works let's say there are the following blueprints:
 *
 *      bp1 = Blueprint<Position, Direction, Rotation> (func1);
 *      bp2 = Blueprint<Position, Direction, Rotation> (func2);
 *      bp3 = Blueprint<Position, Direction, Rotation> (func3);
 *
 * Now a BlueprintInMemory is created by providing the Blueprints in the constructor:
 *
 *      memory = BlueprintsInMemory(bp1, bp2, bp3);
 *
 * This will result in the following structure:
 *
 *      BlueprintsInMemory<Position, Direction, Rotation> - note that the template parameters can be deducted from the constructor args
 *      |
 *      - std::tuple<>
 *      | |- std::vector<Position> { Position(bp1), Position(bp2), Position(bp3) }
 *      | |- std::vector<Direction> { Direction(bp1), Direction(bp2), Direction(bp3) }
 *      | |- std::vector<Rotation> { Rotation(bp1), Rotation(bp2), Rotation(bp3) }
 *      |
 *      - std::vector<std::function<const double, TWorld &, Position &, Direction &, Rotation &>> { func1, func2, func3 }
 *
 * The class provides an update(const double, TWorld &) function which iterates through all functions stored in the 'functions' vector and calls them with
 * the respective 'components' as attributes.
 *
 * Using the above example, a call to:
 *
 *      memory.update(1, world_obj);
 *
 * Will call the following functions:
 *
 *      1. func1 (1, &world_obj, &Position(bp1), &Direction(bp1), &Rotation(bp1));
 *      2. func2 (1, &world_obj, &Position(bp2), &Direction(bp2), &Rotation(bp2));
 *      3. func3 (1, &world_obj, &Position(bp3), &Direction(bp3), &Rotation(bp3));
 *
 * If needed the components can be accessed with use of the 'auto &get(is_a_memory auto &memory)' function declared above.
 * Example:
 *
 *      auto &direction_components = get<Direction>(memory);
 *                                   -> returns &std::vector<Direction> { Direction(bp1), Direction(bp2), Direction(bp3) }
 *
 * @tparam Ts a list of components which adhere to the is_many_components concept (see components.h).
 */
template<is_many_components... Ts>
class BlueprintsInMemory : public MemoryBase {
    using Components = std::tuple<std::vector<Ts>...>;
    using BlueprintUpdateFunction = std::function<void(const double, TWorld &, Ts &...)>;
    using VectorOfUpdateFunctions = std::vector<BlueprintUpdateFunction>;

public:
    Components components;
    VectorOfUpdateFunctions functions;

    static constexpr BlueprintUpdateFunction unpack_function(is_a_blueprint auto &blueprint) { return std::move(blueprint.update_function); }

    template<is_a_component T, is_a_blueprint... Blueprint>
    static constexpr std::vector<T> unpack_components(Blueprint const &...blueprint_pack) { return {std::move(std::get<T>(blueprint_pack.components))...}; }

    template<is_a_blueprint... Blueprints>
    explicit BlueprintsInMemory(Blueprints const &...blueprint_pack)
        : components{unpack_components<Ts>(blueprint_pack...)...},
          functions{unpack_function(blueprint_pack)...} {};

    ~BlueprintsInMemory() override = default;
    void update(double time, TWorld &world) override;
};

template<is_many_components... Ts>
void BlueprintsInMemory<Ts...>::update(double time, TWorld &world) {
    for (typename VectorOfUpdateFunctions::size_type i = 0; i < functions.size(); i++) {
        auto blueprint_update_function = functions[i];
        blueprint_update_function(time, world, std::get<std::vector<Ts>>(components)[i]...);
    }
}

/**
 * Deduction guide which allows to infer the template arguments of BlueprintsInMemory from Blueprint objects provided in the constructor
 */
template<is_a_component... Cs, is_a_blueprint... Bs>
BlueprintsInMemory(Blueprint<Cs...>, Bs...) -> BlueprintsInMemory<Cs...>;

/*
 * BlueprintsInPool is another utility class for handling collections of Blueprint objects.
 * Its main difference to BlueprintsInMemory is that it creates a pool of N (set with the `amount` variable in the constructor) "empty" components & functions.
 * It uses an internal `first_available_index` to maintain available slots of data in its storage.
 *
 * As an example:
 *
 *      auto pool = BlueprintsInPool<Position, Direction, Rotation>(5);
 *
 * Will create the following structure:
 *
 *      BlueprintsInPool
 *      |
 *      |- std::tuple<>
 *      |  |- std::vector<Position>  { Pos1, Pos2, Pos3, Pos4, Pos5 }
 *      |  |- std::vector<Direction> { Dir1, Dir2, Dir3, Dir4, Dir5 }
 *      |  |- std::vector<Rotation>  { Rot1, Rot2, Rot3, Rot4, Rot5 }
 *      |
 *      |- std::vector<std::function<const double, TWorld &, ... >>
 *                                   { fun1, fun2, fun3, fun4, fun5 }
 *                                     *                          ^
 *                                     first_available_index=[0]  size=[5]
 *
 * The class provides functions for adding, removing and update of data:
 *
 *      void add(is_a_blueprint auto &blueprint)
 *      void remove_at(size_t index);
 *      void update(double time, TWorld &world); // works the same as in BulletsInMemory
 *
 * Whenever a Blueprint is added, its components and functions are stored in their respective slots, and the index is increased by 1 (with bounds checking of `size`).
 * Adding two Blueprints "bp1" and "bp2" as an example will modify the structure:
 *
 *      pool.add(bp1);
 *      pool.add(bp2);
 *
 *      BlueprintsInPool
 *      |
 *      |- std::tuple<>
 *      |  |- std::vector<Position>  { Pos(bp1), Pos(bp2), Pos3, Pos4, Pos5 }
 *      |  |- std::vector<Direction> { Dir(bp1), Dir(bp2), Dir3, Dir4, Dir5 }
 *      |  |- std::vector<Rotation>  { Rot(bp1), Rot(bp2), Rot3, Rot4, Rot5 }
 *      |
 *      |- std::vector<std::function<const double, TWorld &, ... >>
 *                                   { fun(bp1), fun(bp2), fun3, fun4, fun5 }
 *                                                         *
 *                                                         first_available_index=[2]    // note how the index increased after adding two blueprints
 *
 * Now removing data at a given index doesn't "remove" it per se, just copies entries from right to left up until the current `first_available_index` value.
 * The `first_available_index` is decreased after removal.
 * This means that deleting the first position in this example will modify the structure in the following way:
 *
 *      pool.remove_at(0);
 *
 *      BlueprintsInPool
 *      |                                       note how both entries at [1] and [2] have the same contents because of shifting from right to left until the index value
 *      |- std::tuple<>                          v     v
 *      |  |- std::vector<Position>  { Pos(bp2), Pos3, Pos3, Pos4, Pos5 }
 *      |  |- std::vector<Direction> { Dir(bp2), Dir3, Dir3, Dir4, Dir5 }
 *      |  |- std::vector<Rotation>  { Rot(bp2), Rot3, Rot3, Rot4, Rot5 }
 *      |
 *      |- std::vector<std::function<const double, TWorld &, ... >>
 *                                   { fun(bp2), fun3, fun3, fun4, fun5 }
 *                                     ^         *
 *                                     |         first_available_index=[1]
 *                                     |
 *                                     data at index [0] in all vectors has now the contents of previously [1]
 *
 * Generally speaking the intention of this class is to allow the adding and removal of lots of objects without the actual use of expensive (de)allocation of memory.
 * For example creation of hundreds of bullets on screen, which are removed after hitting an object or when leaving the screen boundary.
 *
 * @tparam Ts a list of components which adhere to the is_many_components concept (see components.h)
 */
template<is_many_components... Ts>
class BlueprintsInPool : MemoryBase {
    using Components = std::tuple<std::vector<Ts>...>;
    using ComponentFunction = std::function<void(const double, TWorld &, Ts &...)>;
    using ComponentFunctionVector = std::vector<ComponentFunction>;

public:
    Components components;
    ComponentFunctionVector functions;
    size_t first_available_index;
    size_t size;

    explicit BlueprintsInPool(const size_t amount)
        : components{std::make_tuple(std::vector<Ts>(amount)...)},
          functions{std::vector<ComponentFunction>(amount)},
          first_available_index{0},
          size{amount} {};

    void add(is_a_blueprint auto &blueprint);
    void remove_at(typename ComponentFunctionVector::size_type index);
    void update(double time, TWorld &world) override;

private:
    template<is_a_component T>
    constexpr void insert_component_at_index(size_t index, T component) {
        std::get<std::vector<T>>(components)[index] = component;
    }
    template<is_a_component T>
    constexpr void swap_components_at_index(size_t index1, size_t index2) {
        std::get<std::vector<T>>(components)[index1] = std::get<std::vector<T>>(components)[index2];
    }
};

template<is_many_components... Ts>
void BlueprintsInPool<Ts...>::update(double time, TWorld &world) {
    for (typename ComponentFunctionVector::size_type i = 0; i < first_available_index; i++) {
        functions[i](time, world, std::get<std::vector<Ts>>(components)[i]...);
    }
}
template<is_many_components... Ts>
void BlueprintsInPool<Ts...>::add(is_a_blueprint auto &blueprint) {
    if (first_available_index < size) {
        (insert_component_at_index(first_available_index, get<Ts>(blueprint)), ...);
        functions[first_available_index] = blueprint.update_function;
        first_available_index += 1;
    }
}
template<is_many_components... Ts>
void BlueprintsInPool<Ts...>::remove_at(typename ComponentFunctionVector::size_type index) {
    if (index <= size && index <= first_available_index) {
        for (typename ComponentFunctionVector::size_type i = index; i < first_available_index - 1; i++) {
            (swap_components_at_index<Ts>(i, i + 1), ...);
            functions[i] = functions[i + 1];
        }
        first_available_index -= 1;
    }
}
#endif//BASILEVS_BLUEPRINTS_H
