#include "Team.hpp"
#include "src/extern/flecs.h"

Teams::Teams(flecs::world &world) {
    world.module<Teams>("teams");

    world.component<Team>();
    world.component<BelongsTo>();
}
