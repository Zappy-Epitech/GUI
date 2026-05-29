#include "Core.hpp"
#include "src/core/Gui.hpp"
#include "src/core/Raylib.hpp"
#include "src/core/Spatial.hpp"
#include "src/extern/flecs.h"

Core::Core(flecs::world &world) {
    flecs::entity mod = world.module<Core>("core");
    world.import<Spatial>().child_of(mod);
    world.import<Raylib>().child_of(mod);
    world.import<Gui>().child_of(mod);
}
