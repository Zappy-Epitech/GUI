#pragma once
#include "../ZappyProtocol.hpp"

namespace flecs {
struct world;
}

void applyPlayerNew(flecs::world &world, const zappy::PlayerNew &evt);
void applyPlayerPosition(flecs::world &world, zappy::PlayerPosition &evt);
void applyPlayerLevel(flecs::world &world, zappy::PlayerLevel &evt);
void applyPlayerInventory(flecs::world &world, zappy::PlayerInventory &evt);
void applyPlayerBroadcast(flecs::world &world, zappy::PlayerBroadcast &evt);
void applyPlayerResourceDrop(flecs::world &world, zappy::PlayerResourceDrop &evt);
