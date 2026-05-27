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
    world.import<Grid>();

    world.entity("Play Button")
        .set(Button("Play !"))
        .set(Position2::center())
        .observe<OnClick>([](flecs::entity e) {
            Grid::spawn(e.world(), 10, 10);
            e.destruct();
        });

    world.app()
        .target_fps(90)
        .enable_stats()
        .enable_rest()
        .threads(4)
        .run();
}
