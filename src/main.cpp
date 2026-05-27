#include "extern/flecs.hpp"
#include "modules/Grid.hpp"
#include "modules/Gui.hpp"
#include "modules/Raylib.hpp"
#include "modules/Spatial.hpp"
#include <cstdio>
#include <raylib.h>
#include <sys/types.h>

int main() {
    flecs::world world;

    world.import<Spatial>();
    world.import<Raylib>();
    world.import<Gui>();

    spawnGrid(world, 10, 10);

    world.entity("Button")
        .set(Button("Click Me !"))
        .set(Position2{ 10, 10 })
        .observe<OnClick>([](flecs::entity e) {
            puts("Clicked");
        });

    world.app()
        .target_fps(120)
        .enable_stats()
        .enable_rest()
        .threads(4)
        .run();
}
