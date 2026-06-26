#include "AppScenes.hpp"
#include "src/core/Scenes.hpp"
#include "src/extern/flecs.h"
#include "src/scenes/EndGame.hpp"
#include "src/scenes/Game.hpp"
#include "src/scenes/Home.hpp"
#include "src/scenes/Settings.hpp"

/// Registers the concrete application scenes.
AppScenes::AppScenes(flecs::world &world) {
    flecs::entity module = world.module<AppScenes>("app");

    world.import<Home>().child_of(module);
    world.import<SettingsScene>().child_of(module);
    world.import<Game>().child_of(module);
    world.import<EndGame>().child_of(module);

    enterScene<Home>(world);
}
