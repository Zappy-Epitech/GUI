#include "Scenes.hpp"
#include "src/extern/flecs.h"
#include "src/scenes/Home.hpp"

Scenes::Scenes(flecs::world &world) {
    flecs::entity mod = world.module<Scenes>("scenes");
    world.import<Home>().child_of(mod);
}
