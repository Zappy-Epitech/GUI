#include "src/core/Core.hpp"
#include "src/core/Scenes.hpp"
#include "src/core/Spatial.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/GamePlay.hpp"
#include "src/minecraft/Minecraft.hpp"
#include "src/network/NetworkModule.hpp"
#include "src/protocol/command/CommandRunner.hpp"
#include "src/scenes/AppScenes.hpp"
#include "src/scenes/Game.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

struct CommandInput {
    char *command;
};

struct Command {
    Command(flecs::world &world) {
        world.module<Command>("command").set<CommandInput>(CommandInput(strdup("")));

        world.component<CommandInput>("command::CommandInput")
            .member(flecs::String, "enter a command")
            .set<CommandInput>(CommandInput(strdup("")));

        world.observer<const CommandInput>("command::CommandInputObserver")
            .event(flecs::OnSet)
            .each([](flecs::entity e, const CommandInput &input) {
                std::string command(input.command);

                if (isCurrentScene<Game>(e.world())) {
                    runCommand(e.world(), command);
                    e.get_mut<CommandInput>().command[0] = '\0';
                }
            });
    }
};

/// Starts the Zappy GUI application.
int main() {
    flecs::world world;

    world.import<Core>();
    world.import<Minecraft>();
    world.import<GamePlay>();
    world.import<NetworkModule>();
    world.import<AppScenes>();
    world.import<Command>();

    world.app()
        .target_fps(120)
#ifndef NDEBUG
        .enable_stats()
        .enable_rest()
#endif
        .threads(0)
        .run();
}
