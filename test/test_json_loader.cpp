//
// Created by geoco on 05.10.2021.
//
#include <level-loader.h>
#include <gtest/gtest.h>

class LoaderTest : public testing::Test {
protected:
    LoaderTest() = default;
    void SetUp() override {
        auto json_loader = LevelLoader("../assets/json/test/test_level_loader.json");
        enemies_ptr = json_loader.get_enemy_spawns();
    }

    std::shared_ptr<BlueprintsInMemory<ENEMY_COMPONENTS>> enemies_ptr;
};

TEST_F(LoaderTest, EnemiesAreLoadedFromJsonFile) {
    EXPECT_TRUE(enemies_ptr != 0);
}

TEST_F(LoaderTest, CorrectAmountOfEnemiesCreated) {
    EXPECT_EQ(2,enemies_ptr.get()->functions.size());
}

TEST_F(LoaderTest, EnemiesWillSpawnAtProperTimes) {
    auto enemies = enemies_ptr.get();
    auto enemy_activation = std::get<std::vector<components::Activation>>(enemies->components);
    EXPECT_EQ(1, static_cast<int>(enemy_activation[0].activate_after_seconds));
    EXPECT_EQ(2, static_cast<int>(enemy_activation[1].activate_after_seconds));
}

TEST_F(LoaderTest, EnemiesWillSpawnAtProperPositions) {
    auto enemies = enemies_ptr.get();
    auto enemy_position = std::get<std::vector<components::Movement>>(enemies->components);
    EXPECT_EQ(60, enemy_position[0].position.x);
    EXPECT_EQ(-20, enemy_position[0].position.y);
    EXPECT_EQ(10, enemy_position[1].position.x);
    EXPECT_EQ(-20, enemy_position[1].position.y);
}

TEST_F(LoaderTest, EnemiesHaveProperMovement) {
    auto enemies = enemies_ptr.get();
    auto enemy_movement = std::get<std::vector<components::MovementPath>>(enemies->components);
    EXPECT_EQ(3, enemy_movement[0].points.size());
    EXPECT_EQ(10, enemy_movement[0].points[0].x);
    EXPECT_EQ(100, enemy_movement[0].points[0].y);
    EXPECT_EQ(3, enemy_movement[1].points.size());
    EXPECT_EQ(30, enemy_movement[1].points[2].x);
    EXPECT_EQ(70, enemy_movement[1].points[2].y);
}

int main(int ac, char *av[])
{
    testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}