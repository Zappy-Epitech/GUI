#include "Game.hpp"
#include "../../extern/flecs.hpp"
#include "src/modules/CameraController.hpp"
#include "src/modules/Grid.hpp"
#include "src/modules/Gui.hpp"
#include "src/modules/Spatial.hpp"
#include "src/modules/scenes/Home.hpp"

Game::Game(flecs::world &world) {
    world.module<Game>();
    world.import<CameraController>();
    Grid::spawn(world, 10, 10);
    world.entity("Exit Button")
        .set(Button("Exit"))
        .set(Position2::splat(100).with_y(50))
        .set(OnClick([world](flecs::entity) mutable {
            world.entity<Game>().destruct();
            world.import<Home>();
        }));
}
