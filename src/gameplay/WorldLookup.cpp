#include "WorldLookup.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/Grid.hpp"
#include "src/gameplay/Player.hpp"
#include "src/gameplay/Team.hpp"
#include "src/scenes/Game.hpp"
#include "src/core/Scenes.hpp"
#include <format>

/// Finds a player entity by id.
flecs::entity findPlayer(const flecs::world &world, int id) {
    return world.query<PlayerId>()
        .find([id](PlayerId &playerId) {
            return playerId.value == id;
        });
}

/// Finds a tile entity by coordinates.
flecs::entity findTile(const flecs::world &world, int x, int y) {
    return world.query<GridPosition>().find([x, y](GridPosition &pos) {
        return pos.x == x && pos.y == y;
    });
}

/// Finds a team entity by name.
flecs::entity findTeam(const flecs::world &world, const std::string &name) {
    return world.query<Team>()
        .find([name](Team &teamData) {
            return teamData.name == name;
        });
}

/// Finds or creates a team entity.
flecs::entity findOrCreateTeam(const flecs::world &world, const std::string &name) {
    if (flecs::entity team = findTeam(world, name); team) {
        return team;
    }
    return world.entity(std::format("{}", name).c_str())
        .child_of<Teams>()
        .add<DespawnOnExit>(sceneId<Game>(world))
        .set(Team{ name });
}
