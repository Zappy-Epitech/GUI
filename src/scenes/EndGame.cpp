/**
 * @file EndGame.cpp
 * @ingroup gui_scenes
 * @brief Implements the end-game scene: winner summary panel and "Back home" button.
 */
#include "EndGame.hpp"
#include "src/core/Gui.hpp"
#include "src/core/Raylib.hpp"
#include "src/core/Scenes.hpp"
#include "src/core/Spatial.hpp"
#include "src/extern/flecs.h"
#include "src/minecraft/MinecraftRenderer.hpp"
#include "src/network/NetworkModule.hpp"
#include "src/scenes/AppScenes.hpp"
#include "src/scenes/Home.hpp"

#include <algorithm>
#include <format>
#include <raygui.h>
#include <raylib.h>

namespace {

static int resourceTotal(const zappy::Resources &resources) {
    return resources.food + resources.linemate + resources.deraumere + resources.sibur + resources.mendiane + resources.phiras + resources.thystame;
}

static void drawCenteredText(const char *text, int y, int fontSize, Color color) {
    const int width = MeasureText(text, fontSize);
    DrawText(text, GetScreenWidth() / 2 - width / 2, y, fontSize, color);
}

static void drawPlayerRow(const EndGamePlayerStats &player, Rectangle row) {
    DrawRectangleRec(row, Fade(BLACK, 0.35f));
    DrawRectangleLinesEx(row, 1.0f, Fade(WHITE, 0.25f));

    DrawMinecraftHead(player.skin, Rectangle{ row.x + 10, row.y + 8, 40, 40 });
    DrawText(player.name.c_str(), static_cast<int>(row.x + 62), static_cast<int>(row.y + 8), 18, WHITE);
    DrawText(std::format("#{}  Lv {}", player.id, player.level).c_str(), static_cast<int>(row.x + 62), static_cast<int>(row.y + 31), 14, Fade(WHITE, 0.72f));

    const std::string inventory = std::format(
        "food {} | stones {} | total {}",
        player.resources.food,
        resourceTotal(player.resources) - player.resources.food,
        resourceTotal(player.resources));
    DrawText(inventory.c_str(), static_cast<int>(row.x + 220), static_cast<int>(row.y + 20), 16, Fade(WHITE, 0.86f));
}

} // namespace

/// Registers the end-game scene.
EndGame::EndGame(flecs::world &world) {
    auto module = world.module<EndGame>("endgame").child_of<AppScenes>();

    world.component<EndGamePlayerStats>();
    world.component<EndGameState>();
    world.singleton<EndGameState>().set<EndGameState>({}).child_of(module);

    onEnterScene<EndGame>(world, "EnterEndGame", [](flecs::world &world) {
        world.entity("Back Home Button")
            .set(Button("Back home"))
            .set(Position2::center())
            .set(OnClick([](flecs::entity e) {
                flecs::world world = e.world();

                disconnectFromServer(world);
                enterScene<Home>(world);
            }))
            .add<DespawnOnExit>(sceneId<EndGame>(world));
    });

    world.system("DrawEndGame")
        .kind<Render2D>()
        .run([world](flecs::iter &) {
            const EndGameState *state = world.try_get<EndGameState>();
            if (!state) {
                return;
            }

            drawCenteredText("Game Over", 70, 44, WHITE);
            drawCenteredText(std::format("Team {} wins", state->winner).c_str(), 126, 28, GOLD);

            const int count = static_cast<int>(state->players.size());
            drawCenteredText(std::format("{} winning player{}", count, count == 1 ? "" : "s").c_str(), 170, 18, Fade(WHITE, 0.76f));

            const float panelWidth = std::min(760.0f, static_cast<float>(GetScreenWidth()) - 80.0f);
            Rectangle row = { GetScreenWidth() * 0.5f - panelWidth * 0.5f, 220.0f, panelWidth, 58.0f };

            if (state->players.empty()) {
                drawCenteredText("No player stats available for this team", static_cast<int>(row.y + 24), 18, Fade(WHITE, 0.72f));
                return;
            }

            for (const EndGamePlayerStats &player : state->players) {
                if (row.y + row.height > GetScreenHeight() - 120) {
                    break;
                }
                drawPlayerRow(player, row);
                row.y += row.height + 8;
            }
        })
        .add<InScene>(sceneId<EndGame>(world));
}
