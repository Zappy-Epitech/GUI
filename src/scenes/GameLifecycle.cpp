#include "GameLifecycle.hpp"
#include "src/core/Gui.hpp"
#include "src/core/Scenes.hpp"
#include "src/core/Spatial.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/CameraController.hpp"
#include "src/gameplay/Grid.hpp"
#include "src/network/NetworkModule.hpp"
#include "src/protocol/command/CommandRunner.hpp"
#include "src/protocol/command/PlayerCommand.hpp"
#include "src/scenes/Game.hpp"
#include "src/scenes/GameUi.hpp"
#include "src/scenes/Home.hpp"

/// Registers game scene lifecycle observers.
GameLifecycle::GameLifecycle(flecs::world &world) {
    world.module<GameLifecycle>("lifecycle").child_of<Game>();
    world.import<GameUi>();

    onEnterScene<Game>(world, "EnterGame", [](flecs::world &world) {
        world.set<GameUiState>({});
        world.entity<CameraController>().enable();

        world.entity("Exit Button")
            .set(Button("Exit"))
            .set(Position2::splat(100).with_y(25))
            .set(OnClick([](flecs::entity e) {
                flecs::world world = e.world();

                disconnectFromServer(world);
                enterScene<Home>(world);
            }))
            .add<DespawnOnExit>(sceneId<Game>(world));

        world.entity("Command Input")
            .set(TextInput(""))
            .set(Position2::bottom_center())
            .set(OnEnter([](flecs::entity e, std::string &text) {
                flecs::world world = e.world();
                auto &handle = world.get_mut<NetworkClientHandle>();

                if (handle.client && handle.client->getStatus() == net::ClientStatus::Connected) {
                    handle.client->send(text);
                } else {
                    runCommand(world, text);
                }
                text.clear();
            }))
            .add<DespawnOnExit>(sceneId<Game>(world));

        applyPlayerNew(world, zappy::PlayerNew::withIdAndTeam(0, "dream"));
        Grid::spawn(world, 10, 10);
    });

    onExitScene<Game>(world, "ExitGame", [](flecs::world &world) {
        world.entity<CameraController>().disable();
    });
}
