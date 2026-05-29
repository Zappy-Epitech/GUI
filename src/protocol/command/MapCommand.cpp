#include "MapCommand.hpp"
#include "../../extern/flecs.hpp"
#include "src/modules/Spatial.hpp"
#include "src/modules/gameplay/GameAssets.hpp"
#include "src/modules/gameplay/Grid.hpp"
#include <array>
#include <cstdio>
#include <raylib.h>

namespace {

static const std::array<const char *, 7> resourceNames = { "food", "linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame" };

static const std::array<Position3, 7> resourceOffsets = {
    Position3{ 0.00f, 0.00f, 0.00f },
    Position3{ -0.25f, 0.00f, -0.25f },
    Position3{ 0.25f, 0.00f, -0.25f },
    Position3{ -0.25f, 0.00f, 0.00f },
    Position3{ 0.25f, 0.00f, 0.00f },
    Position3{ -0.25f, 0.00f, 0.25f },
    Position3{ 0.25f, 0.00f, 0.25f },
};

static const std::array<int, 7> &resourceAmounts(const zappy::Resources &resources) {
    return *reinterpret_cast<const std::array<int, 7> *>(&resources);
}

static flecs::entity findTile(flecs::world &world, int x, int y) {
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

static void clearTileResources(flecs::entity tile) {
    for (const char *name : resourceNames) {
        flecs::entity resource = tile.lookup(name);

        if (resource) {
            resource.destruct();
        }
    }
}

static void spawnTileResource(
    flecs::world &world,
    flecs::entity tile,
    Vector2 position,
    int resource,
    const Model &model) {
    Position3 base = Grid::topPosition(position.x, position.y);
    Position3 offset = resourceOffsets[resource];

    world.entity()
        .child_of(tile)
        .set_name(resourceNames[resource])
        .set(base.add_x(offset.x).add_y(offset.y).add_z(offset.z))
        .set(model)
        .set(Rotation3::from_xyz(90, 0, 0))
        .set(Scale{ 0.83f });
}

} // namespace

void applyMapNew(flecs::world &world, zappy::MapSize &evt) {
    world.query_builder()
        .with<GridContainer>()
        .build()
        .each([](flecs::entity e) {
            e.destruct();
        });

    Grid::spawn(world, evt.width, evt.height);
}

void applyTileContent(flecs::world &world, zappy::TileContent &evt) {
    flecs::entity tile = findTile(world, evt.x, evt.y);

    if (!tile) {
        return;
    }

    const GameAssets &assets = world.get<GameAssets>();
    const std::array<int, 7> &amounts = resourceAmounts(evt.resources);

    tile.set(evt.resources);
    clearTileResources(tile);

    for (int resourceIndex = 0; resourceIndex < static_cast<int>(amounts.size()); resourceIndex++) {
        if (amounts[resourceIndex] > 0) {
            spawnTileResource(world, tile, Vector2(evt.x, evt.y), resourceIndex, assets.resourceModels[resourceIndex]);
        }
    }
}
