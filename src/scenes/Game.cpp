#include "Game.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/Player.hpp"
#include "src/protocol/command/CommandRunner.hpp"
#include "src/scenes/AppScenes.hpp"
#include "src/scenes/GameLifecycle.hpp"
#include "src/scenes/GameUi.hpp"
#include <raylib.h>

/// Registers the game scene.
Game::Game(flecs::world &world) {
    auto module = world.module<Game>("game").child_of<AppScenes>();

    world.component<Player>()
        .member<int>("level");

    world.component<PlayerId>()
        .member<int>("id");

    world.component<PlayerSkin>();

    world.component<Incantating>();

    world.component<PlayerBroadcastBubble>();

    world.import<GameUi>().child_of(module);
    world.import<GameLifecycle>().child_of(module);

    // onEnterScene<Game>(world, "DebugSetup", [](flecs::world &world) {
    //     world.defer_begin();

    //     runCommand(world, "msz 20 20");
    //     runCommand(world, "pnw #3 5 7 4 1 team1");
    //     runCommand(world, "pin #3 5 7 10 0 0 0 0 0 0");
    //     world.defer_suspend();
    // });
}
