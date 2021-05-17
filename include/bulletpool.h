//
// Created by geoco on 22.04.2021.
//

#ifndef BASILEVS_BULLETPOOL_H
#define BASILEVS_BULLETPOOL_H
/*
#include <cstdint>
#include <vector>
#include <bullets.h>

template<is_a_bullet T>
struct BulletPool {
public:
    BulletPool() = default;
    explicit BulletPool(const uint64_t amount) : pool(amount){};
    int first_available_index{0};
    std::vector<T> pool{1000};
    void add(T &&bullet);
    void removeAt(uint64_t index) {
        if (index < pool.size() && index < first_available_index) {
            for (int i = index; i < first_available_index; i++) {
                pool[i] = pool[i + 1];
            }
            first_available_index--;
        }
    }
};

*/
#endif//BASILEVS_BULLETPOOL_H
