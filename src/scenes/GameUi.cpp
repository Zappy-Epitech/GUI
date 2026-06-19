#include "GameUi.hpp"
#include "src/core/Raylib.hpp"
#include "src/core/Scenes.hpp"
#include "src/core/Spatial.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/Player.hpp"
#include "src/gameplay/Team.hpp"
#include "src/scenes/Game.hpp"

#include <format>
#include <raygui.h>
#include <raylib.h>

/// Registers game UI observers and render systems.
GameUi::GameUi(flecs::world &world) {
    auto module = world.module<GameUi>("ui").child_of<Game>();

    world.singleton<GameUiState>().set<GameUiState>({}).child_of(module);

    world.observer<const Player, const zappy::Resources>("OnSetPlayerResources")
        .event(flecs::OnSet)
        .each([world](flecs::entity e, const Player &p, const zappy::Resources &r) {
            auto &state = world.get_mut<GameUiState>();
            if (e.id() == state.selectedPlayer) {
                state.playerName = e.name().c_str();
                state.level = p.level;
                state.resources = r;
                state.panelPositionX = 0.0f;
            }
        });

    world.observer<const Player>("OnRemovePlayer")
        .event(flecs::OnRemove)
        .each([world](flecs::entity e, const Player &) {
            auto &state = world.get_mut<GameUiState>();
            if (e.id() == state.selectedPlayer)
                state.selectedPlayer = 0;
        });

    world.system<const Team>("DrawTeamButtons")
        .kind<Render2D>()
        .run([world](flecs::iter &it) {
            auto &state = world.get_mut<GameUiState>();
            Rectangle btn = { 200, 25, 200, 42 };

            while (it.next()) {
                auto teams = it.field<const Team>(0);
                for (auto i : it) {
                    if (GuiButton(btn, teams[i].name.c_str())) {
                        state.openedTeam = (state.openedTeam == it.entity(i).id()) ? 0 : it.entity(i).id();
                        state.panelPositionX = btn.x;
                    }
                    btn.x += btn.width + 10;
                }
            }
        })
        .add<InScene>(sceneId<Game>(world));

    world.system<const Player, const zappy::Resources>("DrawPlayerList")
        .kind<Render2D>()
        .run([world](flecs::iter &it) {
            if (auto &state = world.get_mut<GameUiState>(); state.openedTeam != 0) {
                const Rectangle panel = { state.panelPositionX, 72, 200, 240 };

                DrawRectangleLinesEx(panel, 1.0f, Fade(WHITE, 0.35f));

                Rectangle btn = { state.panelPositionX + 8, panel.y + 8, panel.width - 16, 36 };

                while (it.next()) {
                    auto players = it.field<const Player>(0);
                    auto resources = it.field<const zappy::Resources>(1);

                    for (auto i : it) {
                        auto e = it.entity(i);
                        if (e.has<BelongsTo>(state.openedTeam)) {
                            if (GuiButton(btn, e.name().c_str())) {
                                state.selectedPlayer = e.id();
                                state.playerName = e.name().c_str();
                                state.level = players[i].level;
                                state.resources = resources[i];
                            }
                            btn.y += btn.height + 6;
                        }
                    }
                }
            }
        })
        .add<InScene>(sceneId<Game>(world));

    world.system("DrawInventoryModal")
        .kind<Render2D>()
        .run([world](flecs::iter &) {
            if (auto &state = world.get_mut<GameUiState>(); state.selectedPlayer != 0 && state.resources.has_value()) {
                const Rectangle modal = Position2::center(400, 700).rect(400, 700);

                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.55f));

                if (GuiWindowBox(modal, state.playerName.c_str())) {
                    state.selectedPlayer = 0;
                    return;
                }

                const auto &r = state.resources.value();
                float y = modal.y + 30 + 58;

                for (const auto &[label, amount] : {
                         std::pair("level", state.level),
                         std::pair("food", r.food),
                         std::pair("linemate", r.linemate),
                         std::pair("deraumere", r.deraumere),
                         std::pair("sibur", r.sibur),
                         std::pair("mendiane", r.mendiane),
                         std::pair("phiras", r.phiras),
                         std::pair("thystame", r.thystame),
                     }) {
                    GuiLabel(Position2(modal.x + 16, y).rect(modal.width * 0.65f, 58), label);
                    GuiLabel(Position2(modal.x + modal.width - 96, y).rect(80, 58), std::format("{}", amount).c_str());
                    y += 68;
                }

                if (GuiButton(Rectangle{ modal.x + 20, modal.y + modal.height - 62, modal.width - 40, 48 }, "Close"))
                    state.selectedPlayer = 0;
            }
        })
        .add<InScene>(sceneId<Game>(world));


    // world.system("FreqSlider").run([](flecs::iter &) {
    //     GuiSlider({}, const char *textLeft, const char *textRight, float *value, float minValue, float maxValue)
    // });
}
