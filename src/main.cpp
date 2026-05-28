#include "extern/flecs.hpp"
#include "modules/Gui.hpp"
#include "modules/Raylib.hpp"
#include "modules/Spatial.hpp"
#include "modules/gameplay/Grid.hpp"
#include "src/modules/Postframe.hpp"
#include "src/modules/gameplay/CameraController.hpp"
#include "src/modules/gameplay/GameAssets.hpp"
#include "src/modules/rendering/MinecraftSkinRenderer.hpp"
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
    world.import<MinecraftSkinRenderer>();

    GameAssets::spawn(world);

    world.import<Home>();

    world.app()
        .target_fps(90)
        .enable_stats()
        .enable_rest()
        .threads(4)
        .run();
}
