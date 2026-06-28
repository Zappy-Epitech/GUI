/**
 * @file TeamCommand.cpp
 * @ingroup gui_protocol
 * @brief Creates teams in response to team-name protocol events.
 */
#include "TeamCommand.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/WorldLookup.hpp"

/// Applies a team name event.
void applyTeamName(const flecs::world &world, zappy::TeamName &evt) {
    findOrCreateTeam(world, evt.name);
}
