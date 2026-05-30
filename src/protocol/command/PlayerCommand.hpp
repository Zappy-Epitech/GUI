#pragma once
#include "../ZappyProtocol.hpp"

namespace flecs {
struct world;
}

void applyPlayerNew(flecs::world &world, const zappy::PlayerNew &evt);
void applyPlayerPosition(flecs::world &world, zappy::PlayerPosition &evt);
void applyPlayerLevel(flecs::world &world, zappy::PlayerLevel &evt);
void applyPlayerInventory(flecs::world &world, zappy::PlayerInventory &evt);
void applyPlayerExpelled(flecs::world &world, zappy::PlayerExpelled &evt);
void applyPlayerBroadcast(flecs::world &world, zappy::PlayerBroadcast &evt);
void applyIncantationStart(flecs::world &world, zappy::IncantationStart &evt);
void applyPlayerEggLayStart(flecs::world &world, zappy::PlayerEggLayStart &evt);
void applyPlayerResourceDrop(flecs::world &world, zappy::PlayerResourceDrop &evt);
