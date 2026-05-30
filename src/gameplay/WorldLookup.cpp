#include "WorldLookup.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/Grid.hpp"
#include "src/gameplay/Player.hpp"
#include "src/gameplay/Team.hpp"
#include <format>

flecs::entity findPlayer(flecs::world &world, int id) {
    return world.query<PlayerId>()
        .find([id](PlayerId &playerId) {
            return playerId.value == id;
        });
}

flecs::entity findTile(flecs::world &world, int x, int y) {
    return world.query<GridPosition>().find([x, y](GridPosition &pos) {
        return pos.x == x && pos.y == y;
    });
}

flecs::entity findTeam(flecs::world &world, const std::string &name) {
    return world.query<Team>()
        .find([name](Team &teamData) {
            return teamData.name == name;
        });
}

flecs::entity findOrCreateTeam(flecs::world &world, const std::string &name) {
    flecs::entity team = findTeam(world, name);

    if (team) {
        return team;
    }

    return world.entity(std::format("{}", name).c_str())
        .child_of<Teams>()
        .set(Team{ name });
}
