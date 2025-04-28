//
// Created by Patryk Szczypień on 15/04/2025.
//

#ifndef POOL_HPP
#define POOL_HPP
#include <vector>
#include <ranges>
#include <optional>

template<typename T>
class Pool {
public:
    explicit Pool(const size_t amount) : _pool(amount), _pool_size(amount) {}
    explicit Pool(std::vector<T> &&objects) : _pool(std::move(objects)), _pool_size(_pool.size()) {}
    void add(T &&object);
    void remove_at(size_t index);
    std::optional<T> next();
    std::ranges::subrange<typename std::vector<const T>::iterator> get_occupied() const;
    std::vector<T> get_all();
    size_t first_available() const;

private:
    std::vector<T> _pool;
    size_t _pool_size;
    size_t _first_available_index{0};
};

template<typename T>
void Pool<T>::add(T &&object)
{
    if (_first_available_index < _pool_size) {
        _pool[_first_available_index++] = std::forward<T>(object);
    }
}

template<typename T>
void Pool<T>::remove_at(const size_t index)
{
      if (index < _pool_size && index < _first_available_index) {
        for (size_t i = index; i < _first_available_index - 1; i++) {
            _pool[i] = _pool[i + 1];
        }
        --_first_available_index;
    }
}

template<typename T>
std::optional<T> Pool<T>::next()
{
    if (_first_available_index < _pool_size) {
        return std::optional<T>(_pool[_first_available_index++]);
    }
    return std::nullopt;
}

template<typename T>
std::ranges::subrange<typename std::vector<const T>::iterator> Pool<T>::get_occupied() const
{
    return std::ranges::subrange(_pool.begin(), _pool.begin() + _first_available_index);
}

template<typename T>
std::vector<T> Pool<T>::get_all()
{
    return _pool;
}

template<typename T>
size_t Pool<T>::first_available() const
{
    return _first_available_index;
}
#endif //POOL_HPP
