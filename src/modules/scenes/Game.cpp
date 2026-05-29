#include "Game.hpp"
#include "../../extern/flecs.hpp"
#include "src/modules/Gui.hpp"
#include "src/modules/Raylib.hpp"
#include "src/modules/SkinAnimation.hpp"
#include "src/modules/Spatial.hpp"
#include "src/modules/gameplay/CameraController.hpp"
#include "src/modules/gameplay/GamePlay.hpp"
#include "src/modules/gameplay/Grid.hpp"
#include "src/modules/rendering/MinecraftSkinRenderer.hpp"
#include "src/modules/scenes/Home.hpp"
#include "src/protocol/command/CommandRunner.hpp"
#include <raylib.h>
#include <string>

Game::Game(flecs::world &world) {
    world.module<Game>().child_of<GamePlay>();
    world.entity<CameraController>().enable();

    static Model SteveModel = {};

    if (SteveModel.meshCount == 0) {
        SteveModel = LoadModel("./assets/models/steve/scene.gltf");
    }

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
        .set(OnEnter([world](flecs::entity, std::string &text) mutable {
            runCommand(world, text);
            text.clear();
        }));
}
