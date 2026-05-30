#include "Team.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/GamePlay.hpp"

Teams::Teams(flecs::world &world) {
    world.module<Teams>("teams");

    world.component<Team>();
    world.component<BelongsTo>();
}
