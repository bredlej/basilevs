//
// Created by geoco on 05.10.2021.
//
#include <level-loader.h>
#include <gtest/gtest.h>

class LoaderTest : public testing::Test {
protected:
    LoaderTest() = default;
};

TEST_F(LoaderTest, LoadsJsonFromFile) {

    auto json_loader = LevelLoader("C:\\Users\\geoco\\source\\repos\\basilevs\\assets\\json\\level1.json");
    EXPECT_TRUE(json_loader.get_enemy_spawns() != 0);
}

int main(int ac, char *av[])
{
    testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}