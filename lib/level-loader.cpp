//
// Created by geoco on 06.10.2021.
//

#include "level-loader.h"

nlohmann::json LevelLoader::load(const std::string &file_name) const
{
    std::ifstream file(file_name);
    auto level_data = nlohmann::json::parse(file);
    assert(!level_data.is_null());

    return level_data;
}

std::shared_ptr<TWorld::EnemiesInMemory> LevelLoader::get_enemy_spawns()
{
    static constexpr auto get_vector = [](const auto &coordinates)
    { return Vector2{coordinates[0], coordinates[1]}; };
    static constexpr auto get_movements = [](nlohmann::json enemy_data) -> std::deque<Vector2>
    {
        std::deque<Vector2> movements;
        std::ranges::transform(enemy_data[json::kMovement], std::back_inserter(movements), get_vector);
        return movements;
    };
    static constexpr auto get_enemy = [](const auto &spawn_data) -> TWorld::EnemyType
    {
        const auto time = static_cast<double>(spawn_data[json::kTime]);
        const auto position = get_vector(spawn_data[json::kPosition]);
        const std::string type = spawn_data[json::kEnemy][json::kType];
        const auto movements = get_movements(spawn_data[json::kEnemy]);
        const auto definition_function = behaviours::enemy::definitions.find(type)->second;
        const auto definition = definition_function(movements);
        return basilevs::initialization::create_enemy(time, position, definition);
    };

    assert(!data[json::kSpawns].is_null());

    std::vector<TWorld::EnemyType> enemies;
    std::ranges::transform(data[json::kSpawns], std::back_inserter(enemies), get_enemy);

    return std::make_shared<TWorld::EnemiesInMemory>(BlueprintsInMemory(enemies));
}

std::shared_ptr<TWorld::EnemiesInMemory> LevelLoader::get_enemy_spawns(const Core::TextureCache &texture_cache)
{
    static constexpr auto get_vector = [](const auto &coordinates)
    { return Vector2{coordinates[0], coordinates[1]}; };
    static constexpr auto get_movements = [](nlohmann::json enemy_data) -> std::deque<Vector2>
    {
        std::deque<Vector2> movements;
        std::ranges::transform(enemy_data[json::kMovement], std::back_inserter(movements), get_vector);
        return movements;
    };

    static constexpr auto get_enemy = [](const auto &spawn_data, const Core::TextureCache &texture_cache) -> TWorld::EnemyType
    {
        const auto time = static_cast<double>(spawn_data[json::kTime]);
        const auto position = get_vector(spawn_data[json::kPosition]);
        const std::string type = spawn_data[json::kEnemy][json::kType];
        const auto movements = get_movements(spawn_data[json::kEnemy]);
        const auto definition_function = behaviours::enemy::definitions.find(type)->second;
        const auto definition = definition_function(movements);
        return basilevs::initialization::create_enemy_with_sprite(time, texture_cache, position, definition);
    };

    assert(!data[json::kSpawns].is_null());

    std::vector<TWorld::EnemyType> enemies;
    std::ranges::transform(data[json::kSpawns], std::back_inserter(enemies), [&](const auto enemy) { return get_enemy(enemy, texture_cache);});

    return std::make_shared<TWorld::EnemiesInMemory>(BlueprintsInMemory(enemies));
}
