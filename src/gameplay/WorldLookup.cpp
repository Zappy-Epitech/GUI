#include "WorldLookup.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/Grid.hpp"
#include "src/gameplay/Player.hpp"
#include <format>

flecs::entity findPlayer(flecs::world &world, int id) {
    flecs::entity found;

    world.query_builder<const PlayerId>()
        .build()
        .each([&](flecs::entity player, const PlayerId &playerId) {
            if (playerId.value == id) {
                found = player;
            }
        });

    if (found) {
        return found;
    }

    return world.lookup(std::format("Player({})", id).c_str());
}

flecs::entity findTile(flecs::world &world, int x, int y) {
    flecs::entity found;

    world.query_builder<const GridPosition>()
        .build()
        .each([&](flecs::entity tile, const GridPosition &position) {
            if (position.x == x && position.y == y) {
                found = tile;
            }
        });

    return found;
}
