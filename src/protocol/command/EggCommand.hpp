#pragma once
#include "../ZappyProtocol.hpp"

namespace flecs {
struct world;
}

/// Applies a new egg event.
void applyEggNew(flecs::world &world, zappy::EggNew &evt);

/// Applies an egg hatched event.
void applyEggHatched(flecs::world &world, zappy::EggHatched &evt);

/// Applies an egg death event.
void applyEggDeath(flecs::world &world, zappy::EggDeath &evt);
