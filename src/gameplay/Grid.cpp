#include "Grid.hpp"
#include "src/core/Raylib.hpp"
#include "src/core/Spatial.hpp"
#include "src/extern/flecs.h"
#include "src/protocol/ZappyProtocol.hpp"

#include <format>
#include <raylib.h>

static constexpr Size2 cellSize = { 1, 1 };
static constexpr float cellPadding = 0.1f;

Grid::Grid(flecs::world &world) {
    world.module<Grid>("grid");

    world.component<GridContainer>();
    world.component<GridCell>();
    world.component<GridPosition>()
        .member<int>("x")
        .member<int>("y");

    world.system("LoadGridCellModel").kind(flecs::OnStart).run([world](auto) {
        Model model = LoadModel("./assets/models/grass/scene.gltf");

        world.prefab<GridCell>()
            .set(model)
            .set(Scale{ 0.4 });
    });
}

Position3 Grid::position(int x, int y) {
    return Position3{
        static_cast<float>(x) * (cellSize.width + cellPadding),
        0.0f,
        static_cast<float>(y) * (cellSize.height + cellPadding),
    };
}

Position3 Grid::topPosition(int x, int y) {
    return Grid::position(x, y).with_y(0.45f);
}

void Grid::spawn(const flecs::world &world, int width, int height) {
    flecs::entity grid = world.entity(std::format("Grid({}, {})", width, height).c_str()).add<GridContainer>();

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            world.entity(std::format("GridCell({}, {})", x, y).c_str())
                .is_a<GridCell>()
                .child_of(grid)
                .set(GridPosition{ x, y })
                .set<zappy::Resources>({})
                .set(Grid::position(x, y));
        }
    }
}
