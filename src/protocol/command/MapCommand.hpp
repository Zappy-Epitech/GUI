#pragma once
#include "src/protocol/ZappyProtocol.hpp"

namespace flecs {
struct world;
}

/// Applies a new map size event.
void applyMapNew(flecs::world &world, zappy::MapSize &evt);

/// Applies a tile content event.
void applyTileContent(flecs::world &world, zappy::TileContent &evt);

/// Applies an incantation end event.
void applyIncantationEnd(flecs::world &world, zappy::IncantationEnd &evt);
