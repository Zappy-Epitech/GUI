#include "Grid.hpp"
#include "../extern/flecs.hpp"
#include "Spatial.hpp"

#include <format>
#include <raylib.h>

Grid::Grid(flecs::world &world) {
    world.prefab<GridCell>()
        .set(Position2{ 0, 0 })
        .set(GREEN)
        .set(Size2{ 100, 100 });
}

void Grid::spawn(flecs::world &world, uint width, uint height) {
    flecs::entity grid = world.entity(std::format("Grid({}, {})", width, height).c_str());

    for (uint x = 0; x < width; x++) {
        for (uint y = 0; y < height; y++) {
            world.entity(std::format("GridCell({}, {})", x, y).c_str()).is_a<GridCell>().child_of(grid);
        }
    }
}
