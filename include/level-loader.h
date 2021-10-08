//
// Created by geoco on 06.10.2021.
//

#ifndef BASILEVS_LEVEL_LOADER_H
#define BASILEVS_LEVEL_LOADER_H

#include <fstream>
#include <nlohmann/json.hpp>
#include "basilevs.h"
#include <components.h>
#include <state_handling.h>
#include <blueprints.h>
#include <ranges>

namespace json {
    constexpr auto kSpawns = "spawns";
    constexpr auto kTime = "time";
    constexpr auto kPosition = "position";
    constexpr auto kEnemy = "enemy";
    constexpr auto kType = "type";
    constexpr auto kMovement = "movement";
}

struct LevelLoader {
public:
    explicit LevelLoader(const std::string &file_name) : data{load(file_name)} {};
    std::shared_ptr<TWorld::EnemiesInMemory> get_enemy_spawns();
    std::shared_ptr<TWorld::EnemiesInMemory> get_enemy_spawns(std::vector<Texture2D> &textures);
private:
    [[nodiscard]] nlohmann::json load(const std::string &file_name) const;
    nlohmann::json data;
};
#endif//BASILEVS_LEVEL_LOADER_H
