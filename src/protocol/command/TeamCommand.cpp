#include "TeamCommand.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/WorldLookup.hpp"

void applyTeamName(flecs::world &world, zappy::TeamName &evt) {
    findOrCreateTeam(world, evt.name);
}
