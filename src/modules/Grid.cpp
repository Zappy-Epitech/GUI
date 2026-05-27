#include "Grid.hpp"
#include "../extern/flecs.hpp"
#include "Raylib.hpp"
#include "Spatial.hpp"

#include <format>
#include <raylib.h>

static constexpr Size2 cellSize = { 1, 1 };
static constexpr float cellPadding = 0.1f;

Grid::Grid(flecs::world &world) {
    world.component<GridCell>();

    world.prefab<GridCell>()
        .set(GREEN)
        .set(cellSize)
        .add<Cube>();
}

void Grid::spawn(const flecs::world &world, uint width, uint height) {
    flecs::entity grid = world.entity(std::format("Grid({}, {})", width, height).c_str());

    for (uint x = 0; x < width; x++) {
        for (uint y = 0; y < height; y++) {
            float xPosition = static_cast<float>(x) * (cellSize.width + cellPadding);
            float yPosition = static_cast<float>(y) * (cellSize.height + cellPadding);

            world.entity(std::format("GridCell({}, {})", x, y).c_str())
                .is_a<GridCell>()
                .child_of(grid)
                .set(Position3{ xPosition, 0.0f, yPosition })
                .set(GREEN)
                .set(cellSize)
                .add<Cube>();
        }
    }
}
