#include <raylib-cpp.hpp>
#include <raylib.hpp>
#ifdef __cplusplus
extern "C" {
#endif
#include "raylib.h"
#ifdef __cplusplus
}
#endif
#include <algorithm>
#include "include/basilevs-lib.h"

int main() {
    using namespace basilevs;
    auto window = raylib::Window{config::kScreenWidth, config::kScreenHeight, "Basilevs"};
    auto audio = raylib::AudioDevice();
    basilevs::Game::run(window, audio);
    return 0;
}