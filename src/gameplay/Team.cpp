/**
 * @file Team.cpp
 * @ingroup gui_gameplay
 * @brief Implements registration of team components.
 */
#include "Team.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/GamePlay.hpp"

/// Registers team components.
Teams::Teams(flecs::world &world) {
    world.module<Teams>("teams");

    world.component<Team>();
    world.component<BelongsTo>();
}
