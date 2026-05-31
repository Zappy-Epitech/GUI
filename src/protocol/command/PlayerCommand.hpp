#pragma once
#include "../ZappyProtocol.hpp"

namespace flecs {
struct world;
}

/// Applies a new player event.
void applyPlayerNew(flecs::world &world, const zappy::PlayerNew &evt);

/// Applies a player position event.
void applyPlayerPosition(flecs::world &world, zappy::PlayerPosition &evt);

/// Applies a player level event.
void applyPlayerLevel(flecs::world &world, zappy::PlayerLevel &evt);

/// Applies a player inventory event.
void applyPlayerInventory(flecs::world &world, zappy::PlayerInventory &evt);

/// Applies a player expulsion event.
void applyPlayerExpelled(flecs::world &world, zappy::PlayerExpelled &evt);

/// Applies a player broadcast event.
void applyPlayerBroadcast(flecs::world &world, zappy::PlayerBroadcast &evt);

/// Applies an incantation start event.
void applyIncantationStart(flecs::world &world, zappy::IncantationStart &evt);

/// Applies an egg laying start event.
void applyPlayerEggLayStart(flecs::world &world, zappy::PlayerEggLayStart &evt);

/// Applies a player resource drop event.
void applyPlayerResourceDrop(flecs::world &world, zappy::PlayerResourceDrop &evt);
