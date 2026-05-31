#include "Scenes.hpp"
#include "src/extern/flecs.h"
#include "src/scenes/Home.hpp"

/// Registers the initial scene.
Scenes::Scenes(flecs::world &world) {
    flecs::entity mod = world.module<Scenes>("scenes");
    world.import<Home>().child_of(mod);
}
