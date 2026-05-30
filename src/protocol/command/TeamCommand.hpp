#pragma once

#include "src/protocol/ZappyProtocol.hpp"

namespace flecs {
struct world;
}

void applyTeamName(flecs::world &world, zappy::TeamName &evt);
