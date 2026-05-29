#include "MapCommand.hpp"
#include "../../extern/flecs.hpp"
#include "src/modules/gameplay/Grid.hpp"
#include <cstdio>

void applyMapNew(flecs::world &world, zappy::MapSize &evt) {
    world.query_builder()
        .with<GridContainer>()
        .build()
        .each([](flecs::entity e) {
            e.destruct();
        });

    Grid::spawn(world, evt.width, evt.height);
}
