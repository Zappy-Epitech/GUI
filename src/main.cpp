#include "extern/flecs.hpp"
#include "modules/Grid.hpp"
#include "modules/Gui.hpp"
#include "modules/Raylib.hpp"
#include "modules/Spatial.hpp"
#include "src/modules/CameraController.hpp"
#include <cstdio>
#include <raylib.h>
#include <sys/types.h>

const char *PlayButton = "PlayButton";
const char *IpInput = "IpInput";

int main() {
    flecs::world world;

    world.import<Spatial>();
    world.import<Raylib>();
    world.import<Gui>();
    world.import<Grid>();

    world.entity<CameraController>().disable();

    world.entity(PlayButton)
        .set(Button("Play !"))
        .set(Position2::center())
        .observe<OnClick>([](flecs::entity e) {
            Grid::spawn(e.world(), 10, 10);
            e.destruct();
            e.world().lookup(IpInput).destruct();
            e.world().entity<CameraController>().enable();
        });

    world.entity(IpInput)
        .set(TextInput("Enter IP"))
        .set(Position2::center().sub_y(100))
        .observe<TextUpdated>([](const TextUpdated &update) {
            printf("Text updated: %s\n", update.text.c_str());
        });

    world.app()
        .target_fps(90)
        .enable_stats()
        .enable_rest()
        .threads(4)
        .run();
}
