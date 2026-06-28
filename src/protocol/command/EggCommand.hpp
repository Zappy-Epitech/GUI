/**
 * @file EggCommand.hpp
 * @ingroup gui_protocol
 * @brief Handlers that apply egg-related protocol events to the world.
 */
#pragma once
#include "../ZappyProtocol.hpp"

namespace flecs {
struct world;
}

/// Applies a new egg event.
void applyEggNew(const flecs::world &world, zappy::EggNew &evt);

/// Applies an egg hatched event.
void applyEggHatched(const flecs::world &world, zappy::EggHatched &evt);

/// Applies an egg death event.
void applyEggDeath(const flecs::world &world, zappy::EggDeath &evt);
