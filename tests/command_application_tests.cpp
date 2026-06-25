#include "src/core/Core.hpp"
#include "src/core/Gui.hpp"
#include "src/core/Spatial.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/Egg.hpp"
#include "src/gameplay/GameAssets.hpp"
#include "src/gameplay/Grid.hpp"
#include "src/gameplay/Movement.hpp"
#include "src/gameplay/Player.hpp"
#include "src/gameplay/Simulation.hpp"
#include "src/gameplay/Team.hpp"
#include "src/gameplay/WorldLookup.hpp"
#include "src/minecraft/MinecraftRenderer.hpp"
#include "src/protocol/ZappyProtocol.hpp"
#include "src/protocol/command/CommandRunner.hpp"
#include "src/scenes/EndGame.hpp"
#include "src/scenes/Game.hpp"
#include "src/scenes/GameUi.hpp"
#include <criterion/criterion.h>
#include <format>
#include <raylib.h>
#include <string>

namespace {

static flecs::world makeWorld() {
    flecs::world world;

    world.import<Grid>();
    world.import<Teams>();
    world.component<Player>();
    world.component<PlayerId>();
    world.component<PlayerSkin>();
    world.component<EggId>();
    world.component<Incantating>();
    world.component<ScreenMessage>();
    world.component<Lifetime>();
    world.component<Color>();
    world.component<EndGameState>();
    world.component<GameUiState>();
    world.set<GameUiState>({});

    GameAssets assets;
    assets.skins.push_back(SkinAsset{ "default", Texture2D{} });
    assets.skins.push_back(SkinAsset{ "alternate", Texture2D{ .id = 42 } });
    world.set<GameAssets>(assets);
    return world;
}

static void run(flecs::world &world, const char *line) {
    std::string command(line);
    runCommand(world, command);
}

static bool hasMessage(flecs::world &world, const std::string &text) {
    bool found = false;

    world.query_builder<const ScreenMessage>()
        .build()
        .each([&](const ScreenMessage &message) {
            if (message.value == text) {
                found = true;
            }
        });
    return found;
}

static void spawnPlayer(flecs::world &world, int id, const char *team = "red") {
    run(world, std::format("pnw #{} 0 0 1 1 {}", id, team).c_str());
}

} // namespace

Test(command_application, applies_map_size_and_tile_content) {
    flecs::world world = makeWorld();

    run(world, "msz 2 2");
    run(world, "bct 1 1 7 1 2 3 4 5 6");

    flecs::entity tile = findTile(world, 1, 1);
    cr_assert(tile);

    const auto *resources = tile.try_get<zappy::Resources>();
    cr_assert_not_null(resources);
    cr_assert_eq(resources->food, 7);
    cr_assert_eq(resources->linemate, 1);
    cr_assert_eq(resources->thystame, 6);
}

Test(command_application, applies_team_and_player_creation) {
    flecs::world world = makeWorld();

    run(world, "tna red");
    run(world, "pnw #1 2 3 2 4 red");

    flecs::entity team = findTeam(world, "red");
    flecs::entity player = findPlayer(world, 1);
    cr_assert(team);
    cr_assert(player);
    cr_assert(player.has<BelongsTo>(team));

    cr_assert_eq(player.get<Player>().level, 4);
    cr_assert_eq(player.get<PlayerId>().value, 1);
    cr_assert_eq(player.get<PlayerSkin>().index, 1);
    cr_assert_eq(player.get<Texture2D>().id, 42);
    cr_assert_eq(player.get<Orientation>(), Orientation::EAST);
}

Test(command_application, applies_player_position_level_and_inventory) {
    flecs::world world = makeWorld();
    spawnPlayer(world, 2);

    run(world, "ppo #2 3 4 2");
    run(world, "plv #2 6");
    run(world, "pin #2 5 6 1 2 3 4 5 6 7");

    flecs::entity player = findPlayer(world, 2);
    cr_assert(player);
    cr_assert_eq(player.get<Player>().level, 6);
    cr_assert_eq(player.get<Orientation>(), Orientation::EAST);

    const auto &position = player.get<Position3>();
    Position3 expected = Grid::position(5, 6).with_y(1.0f);
    cr_assert_float_eq(position.x, expected.x, 0.001f);
    cr_assert_float_eq(position.z, expected.z, 0.001f);

    const auto &resources = player.get<zappy::Resources>();
    cr_assert_eq(resources.food, 1);
    cr_assert_eq(resources.thystame, 7);
}

Test(command_application, applies_player_feedback_messages) {
    flecs::world world = makeWorld();
    spawnPlayer(world, 3);

    run(world, "pex #3");
    run(world, "pbc #3 hello team");
    run(world, "pdr #3 0");

    cr_assert(hasMessage(world, "Player #3 was expelled"));
    cr_assert(hasMessage(world, "Player #3: hello team"));
    cr_assert(hasMessage(world, "Player #3 dropped food"));
}

Test(command_application, applies_incantation_start_and_end) {
    flecs::world world = makeWorld();
    run(world, "msz 2 2");
    spawnPlayer(world, 4);
    run(world, "pin #4 0 0 0 0 0 0 0 0 0");

    run(world, "pic 0 0 2 #4");
    flecs::entity player = findPlayer(world, 4);
    cr_assert(player.has<Incantating>());
    cr_assert(hasMessage(world, "Incantation level 2 at (0, 0)"));

    run(world, "pie 0 0 1");
    cr_assert(!player.has<Incantating>());
    cr_assert(hasMessage(world, "Incantation at (0, 0) succeeded"));
}

Test(command_application, applies_player_resource_collect) {
    flecs::world world = makeWorld();
    spawnPlayer(world, 6);

    run(world, "pgt #6 3");

    cr_assert(hasMessage(world, "Player #6 collected sibur"));
}

Test(command_application, applies_player_death) {
    flecs::world world = makeWorld();
    spawnPlayer(world, 7);

    run(world, "pdi #7");

    cr_assert(!findPlayer(world, 7));
    cr_assert(hasMessage(world, "Player #7 died"));
}

Test(command_application, applies_egg_new) {
    flecs::world world = makeWorld();
    run(world, "msz 2 2");
    spawnPlayer(world, 2);
    run(world, "pfk #2");

    run(world, "enw #7 #2 1 1");

    cr_assert(!world.lookup("EggPreview(2)"));
    flecs::entity egg = findEgg(world, 7);
    cr_assert(egg);
    cr_assert_eq(egg.get<EggId>().value, 7);
    cr_assert(egg.has<Model>());
    cr_assert(hasMessage(world, "Egg #7 laid at (1, 1) by player #2"));
}

Test(command_application, applies_egg_hatched_and_death) {
    flecs::world world = makeWorld();
    run(world, "msz 2 2");
    run(world, "enw #3 #1 0 0");

    run(world, "ebo #3");
    cr_assert(!findEgg(world, 3));
    cr_assert(hasMessage(world, "Egg #3 hatched"));

    run(world, "enw #4 #1 1 1");
    run(world, "edi #4");
    cr_assert(!findEgg(world, 4));
    cr_assert(hasMessage(world, "Egg #4 died"));
}

Test(command_application, applies_player_egg_lay_start) {
    flecs::world world = makeWorld();
    spawnPlayer(world, 5);

    run(world, "pfk #5");

    flecs::entity egg = world.lookup("EggPreview(5)");
    cr_assert(egg);
    cr_assert(egg.has<Model>());
    cr_assert(egg.has<Scale>());
    cr_assert(egg.has<Lifetime>());
    cr_assert(hasMessage(world, "Player #5 is laying an egg"));
}

Test(command_application, applies_time_unit) {
    flecs::world world = makeWorld();

    run(world, "sgt 100");
    cr_assert_eq(world.get<SimulationTime>().timeUnit, 100);

    run(world, "sst 42");
    cr_assert_eq(world.get<SimulationTime>().timeUnit, 42);
    cr_assert_eq(world.get<GameUiState>().confirmedFrequency, 42);
    cr_assert_float_eq(world.get<GameUiState>().requestedFrequency, 42.0f, 0.01f);
    cr_assert(hasMessage(world, "Time unit set to 42"));
}

Test(command_application, applies_game_end) {
    flecs::world world = makeWorld();
    run(world, "tna TeamB");
    spawnPlayer(world, 8, "TeamB");
    run(world, "plv #8 4");
    run(world, "pin #8 1 1 3 1 2 3 4 5 6");

    run(world, "seg TeamB");

    const auto &result = world.get<GameResult>();
    cr_assert(result.finished);
    cr_assert_eq(result.winner, std::string("TeamB"));
    const auto &endGame = world.get<EndGameState>();
    cr_assert_eq(endGame.winner, std::string("TeamB"));
    cr_assert_eq(endGame.players.size(), 1);
    cr_assert_eq(endGame.players[0].id, 8);
    cr_assert_eq(endGame.players[0].level, 4);
    cr_assert_eq(endGame.players[0].resources.food, 3);
    cr_assert(hasMessage(world, "Team TeamB wins!"));
}

Test(command_application, applies_server_status_messages) {
    flecs::world world = makeWorld();

    run(world, "smg maintenance soon");
    run(world, "suc");
    run(world, "sbp");

    cr_assert(hasMessage(world, "Server: maintenance soon"));
    cr_assert(hasMessage(world, "Server: unknown command"));
    cr_assert(hasMessage(world, "Server: bad parameter"));
}
