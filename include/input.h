//
// Created by geoco on 23.06.2021.
//

#ifndef BASILEVS_INPUT_H
#define BASILEVS_INPUT_H
#include <bitset>
namespace input
{
    enum class PlayerInput : std::uint8_t
    {
        Left,
        Right,
        Up,
        Down,
        Shoot,
        size
    };

    enum class GameInput : std::uint8_t
    {
        Restart,
        size
    };

    template<typename T>
    concept InputType = requires
    {
        std::is_enum<T>();
    };

    template<InputType T>
    class UserInput {
        using UnderlyingT = typename std::make_unsigned_t<typename std::underlying_type_t<T>>;

    public:
        UserInput &set(T input, bool value = true)
        {
            input_.set(underlying(input), value);
            return *this;
        }
        UserInput &reset()
        {
            input_.reset();
            return *this;
        }
        constexpr bool operator[](T input) const
        {
            return input_[underlying(input)];
        }

    private:
        static constexpr UnderlyingT underlying(T e)
        {
            return static_cast<UnderlyingT>(e);
        }
        std::bitset<underlying(T::size)> input_;
    };

    void register_input (const std::initializer_list<int32_t> keys, const auto action, auto &input)
    {
        bool is_key_down = false;
        bool is_key_up = false;
        for (auto key : keys) {
            is_key_down |= IsKeyDown(key);
            is_key_up |= IsKeyUp(key);
        }
        if (is_key_down) {
            input.set(action);
        } else if (is_key_up) {
            input.set(action, false);
        }
    };
}// namespace input
#endif//BASILEVS_INPUT_H
