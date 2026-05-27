#include "extern/flecs.hpp"
#include "modules/Grid.hpp"
#include "modules/Gui.hpp"
#include "modules/Raylib.hpp"
#include "modules/Spatial.hpp"
#include "src/modules/CameraController.hpp"
#include "src/modules/Postframe.hpp"
#include "src/modules/scenes/Home.hpp"
#include <cstdio>
#include <raylib.h>
#include <sys/types.h>

int main() {
    flecs::world world;

    world.import<Later>();
    world.import<Spatial>();
    world.import<Raylib>();
    world.import<Gui>();
    world.import<Grid>();

    world.import<Home>();

    world.app()
        .target_fps(90)
        .enable_stats()
        .enable_rest()
        .threads(4)
        .run();
}
