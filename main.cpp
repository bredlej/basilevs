#ifdef __cplusplus
extern "C" {
#endif
#include "raylib.h"
#ifdef __cplusplus
}
#endif
#include "include/basilevs-lib.h"
#include <algorithm>
#include <boost/sml/sml.hpp>

int main() {

    basilevs::Game::run();

    return 0;
}
