#include "WorldLookup.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/Grid.hpp"
#include "src/gameplay/Player.hpp"
#include "src/gameplay/Team.hpp"
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

flecs::entity findTeam(flecs::world &world, const std::string &name) {
    flecs::entity found;

    world.query_builder<const Team>()
        .build()
        .each([&](flecs::entity team, const Team &teamData) {
            if (teamData.name == name) {
                found = team;
            }
        });

    if (found) {
        return found;
    }

    return world.lookup(std::format("Team({})", name).c_str());
}

flecs::entity findOrCreateTeam(flecs::world &world, const std::string &name) {
    flecs::entity team = findTeam(world, name);

    if (team) {
        return team;
    }

    return world.entity(std::format("Team({})", name).c_str())
        .set(Team{ name });
}
