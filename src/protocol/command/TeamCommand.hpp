/**
 * @file TeamCommand.hpp
 * @ingroup gui_protocol
 * @brief Handler that applies team-name protocol events to the world.
 */
#pragma once

#include "src/protocol/ZappyProtocol.hpp"

namespace flecs {
struct world;
}

/// Applies a team name event.
void applyTeamName(const flecs::world &world, zappy::TeamName &evt);
