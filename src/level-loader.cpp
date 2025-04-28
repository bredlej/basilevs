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

void LevelLoader::load_enemy_spawns(const Core::TextureCache &texture_cache, entt::registry &registry)
{
    static constexpr auto get_position = [](const auto &coordinates){ return Vector2{coordinates[0], coordinates[1]}; };
    static constexpr auto get_movements = [](nlohmann::json enemy_data) -> std::deque<Vector2>
    {
        std::deque<Vector2> movements;
        std::ranges::transform(enemy_data[json::kMovement], std::back_inserter(movements), get_position);
        return movements;
    };

    static auto build_enemy_entity = [&texture_cache, &registry](const auto &spawn_data)
    {
        const auto time = static_cast<double>(spawn_data[json::kTime]);
        const auto position = get_position(spawn_data[json::kPosition]);
        const std::string type = spawn_data[json::kEnemy][json::kType];
        const auto movements = get_movements(spawn_data[json::kEnemy]);
        const auto definition_function = behaviours::enemy::definitions.find(type)->second;
        const auto definition = definition_function(movements);
        basilevs::initialization::create_enemy_with_sprite_entt(registry, time, texture_cache, position, definition);
    };

    assert(!data[json::kSpawns].is_null());

    for (const auto& spawn_data: data[json::kSpawns]) {
        build_enemy_entity(spawn_data);
    }
}
