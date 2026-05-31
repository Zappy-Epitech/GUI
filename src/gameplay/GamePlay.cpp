#include "GamePlay.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/CameraController.hpp"
#include "src/gameplay/GameAssets.hpp"
#include "src/gameplay/Grid.hpp"
#include "src/gameplay/Movement.hpp"
#include "src/gameplay/Team.hpp"

/// Imports gameplay modules.
GamePlay::GamePlay(flecs::world &world) {
    flecs::entity module = world.module<GamePlay>("gameplay");
    world.import<Grid>().child_of(module);
    world.import<Teams>().child_of(module);
    world.import<CameraController>().disable();
    world.import<Movement>();

    GameAssets::load(world);
}
