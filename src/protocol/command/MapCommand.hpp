#pragma once
#include "src/protocol/ZappyProtocol.hpp"

namespace flecs {
struct world;
}

void applyMapNew(flecs::world &world, zappy::MapSize &evt);
void applyTileContent(flecs::world &world, zappy::TileContent &evt);
