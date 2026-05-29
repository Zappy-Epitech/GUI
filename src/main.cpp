#include "src/extern/flecs.h"
#include "src/core/Core.hpp"
#include "src/minecraft/Minecraft.hpp"
#include "src/gameplay/GamePlay.hpp"
#include "src/scenes/Scenes.hpp"

int main() {
    flecs::world world;

    world.import<Core>();
    world.import<Minecraft>();
    world.import<GamePlay>();
    world.import<Scenes>();

    world.shrink();
    world.app()
        .target_fps(120)
        .enable_stats()
        .enable_rest()
        .threads(0)
        .run();
}
