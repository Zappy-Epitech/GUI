#pragma once
#include "../ZappyProtocol.hpp"

namespace flecs {
struct world;
}

void applyPlayerNew(flecs::world &world, zappy::PlayerNew &evt);
void applyPlayerPosition(flecs::world &world, zappy::PlayerPosition &evt);
void applyPlayerLevel(flecs::world &world, zappy::PlayerLevel &evt);
void applyPlayerInventory(flecs::world &world, zappy::PlayerInventory &evt);
