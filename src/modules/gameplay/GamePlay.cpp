#include "GamePlay.hpp"
#include "../../extern/flecs.hpp"
#include "src/modules/gameplay/CameraController.hpp"
#include "src/modules/gameplay/GameAssets.hpp"
#include "src/modules/gameplay/Grid.hpp"
#include "src/modules/scenes/Home.hpp"

GamePlay::GamePlay(flecs::world &world) {
    flecs::entity module = world.module<GamePlay>();
    world.import<CameraController>().child_of(module).disable();
    world.import<Grid>().child_of(module);
    world.import<Home>().child_of(module);
    GameAssets::load(world);
}
