#include "Home.hpp"
#include "src/core/Gui.hpp"
#include "src/core/Spatial.hpp"
#include "src/extern/flecs.h"
#include "src/scenes/Game.hpp"
#include "src/scenes/Scenes.hpp"

/// Registers the home scene.
Home::Home(flecs::world &world) {
    world.module<Home>("home").child_of<Scenes>();
    world.entity("Play Button")
        .set(Button("Play !"))
        .set(Position2::center())
        .set(OnClick([](flecs::entity e) {
            e.world().entity<Home>().destruct();
            e.world().import<Game>().child_of<Scenes>();
        }));

    world.entity("Ip Input")
        .set(TextInput("Enter IP"))
        .set(Position2::center().sub_y(100))
        .set(OnTextUpdate([](flecs::entity, std::string &update) {
            printf("Text updated: %s\n", update.c_str());
        }));
}
