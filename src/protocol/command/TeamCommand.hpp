#pragma once

#include "src/protocol/ZappyProtocol.hpp"

namespace flecs {
struct world;
}

/// Applies a team name event.
void applyTeamName(const flecs::world &world, zappy::TeamName &evt);
