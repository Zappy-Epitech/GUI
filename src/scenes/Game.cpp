#include "Game.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/Player.hpp"
#include "src/scenes/AppScenes.hpp"
#include "src/scenes/GameLifecycle.hpp"
#include "src/scenes/GameUi.hpp"

/// Registers the game scene.
Game::Game(flecs::world &world) {
    auto module = world.module<Game>("game").child_of<AppScenes>();

    world.component<Player>()
        .member<int>("level");

    world.component<PlayerId>()
        .member<int>("id");

    world.component<Incantating>();

    world.import<GameUi>().child_of(module);
    world.import<GameLifecycle>().child_of(module);
}
