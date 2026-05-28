#include "Game.hpp"
#include "../../extern/flecs.hpp"
#include "src/modules/CameraController.hpp"
#include "src/modules/Grid.hpp"
#include "src/modules/Gui.hpp"
#include "src/modules/Raylib.hpp"
#include "src/modules/Spatial.hpp"
#include "src/modules/scenes/Home.hpp"
#include <raylib.h>

Game::Game(flecs::world &world) {
    world.module<Game>();
    world.import<CameraController>();

    static Model SteveModel = {};

    if (SteveModel.meshCount == 0) {
        SteveModel = LoadModel("./assets/models/steve/scene.gltf");
    }

    world.entity()
        .set(SteveModel)
        .set(Scale{ 0.03f })
        .set(Position3::zero().with_y(1.2));

    Grid::spawn(world, 10, 10);

    world.entity("Exit Button")
        .set(Button("Exit"))
        .set(Position2::splat(100).with_y(50))
        .set(OnClick([world](flecs::entity) mutable {
            world.entity<Game>().destruct();
            world.import<Home>();
        }));

    world.entity("Command Input")
        .set(TextInput(""))
        .set(Position2::bottom_center())
        .set(OnEnter([](flecs::entity, std::string &text) {
            printf("Enter pressed: %s\n", text.c_str());
            text.clear();
        }));
}
