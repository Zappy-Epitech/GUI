#include "extern/flecs.hpp"
#include "modules/Gui.hpp"
#include "modules/Raylib.hpp"
#include "modules/Spatial.hpp"
#include "modules/gameplay/Grid.hpp"
#include "src/modules/gameplay/CameraController.hpp"
#include "src/modules/gameplay/GameAssets.hpp"
#include "src/modules/gameplay/GamePlay.hpp"
#include "src/modules/minecraft/Minecraft.hpp"
#include "src/modules/rendering/MinecraftSkinRenderer.hpp"
#include "src/modules/scenes/Home.hpp"
#include <cstdio>
#include <raylib.h>
#include <sys/types.h>

int main() {
    flecs::world world;

    world.import<Spatial>();
    world.import<Raylib>();
    world.import<Gui>();
    world.import<Minecraft>();
    world.import<GamePlay>();

    world.import<Home>();

    world.app()
        .target_fps(120)
        .enable_stats()
        .enable_rest()
        .threads(0)
        .run();
}
