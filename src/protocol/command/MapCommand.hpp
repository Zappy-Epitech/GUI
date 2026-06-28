/**
 * @file MapCommand.hpp
 * @ingroup gui_protocol
 * @brief Handlers that apply map and tile protocol events to the world.
 */
#pragma once
#include "src/protocol/ZappyProtocol.hpp"

namespace flecs {
struct world;
}

/// Applies a new map size event.
void applyMapNew(const flecs::world &world, zappy::MapSize &evt);

/// Applies a tile content event.
void applyTileContent(const flecs::world &world, zappy::TileContent &evt);

/// Applies an incantation end event.
void applyIncantationEnd(const flecs::world &world, zappy::IncantationEnd &evt);
