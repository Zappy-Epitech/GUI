#pragma once
#include "../ZappyProtocol.hpp"

namespace flecs {
struct world;
}

void applyPlayerNew(flecs::world &world, zappy::PlayerNew &evt);
