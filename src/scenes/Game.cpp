#include "Game.hpp"
#include "src/core/Gui.hpp"
#include "src/core/Raylib.hpp"
#include "src/core/Spatial.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/CameraController.hpp"
#include "src/gameplay/GamePlay.hpp"
#include "src/gameplay/Grid.hpp"
#include "src/gameplay/Player.hpp"
#include "src/gameplay/Team.hpp"
#include "src/protocol/ZappyProtocol.hpp"
#include "src/protocol/command/CommandRunner.hpp"
#include "src/protocol/command/PlayerCommand.hpp"
#include "src/scenes/Home.hpp"
#include "src/scenes/Scenes.hpp"
#include <format>
#include <optional>
#include <raygui.h>
#include <raylib.h>
#include <string>

struct ListState {
    flecs::entity_t openedTeam = 0;
    flecs::entity_t selectedPlayer = 0;
    std::optional<zappy::Resources> resources;
    int level = 0;
    std::string playerName;
};

Game::Game(flecs::world &world) {
    world.module<Game>("game").child_of<Scenes>();

    world.component<Player>().member<int>("level");
    world.component<PlayerId>().member<int>("id");
    world.component<Incantating>();
    world.entity<CameraController>().enable();

    world.set<ListState>({});

    world.observer<const Player, const zappy::Resources>()
        .event(flecs::OnSet)
        .each([world](flecs::entity e, const Player &p, const zappy::Resources &r) {
            auto &state = world.get_mut<ListState>();
            if (e.id() == state.selectedPlayer) {
                state.playerName = e.name().c_str();
                state.level = p.level;
                state.resources = r;
            }
        });

    world.observer<const Player>()
        .event(flecs::OnRemove)
        .each([world](flecs::entity e, const Player &) {
            auto &state = world.get_mut<ListState>();
            if (e.id() == state.selectedPlayer)
                state.selectedPlayer = 0;
        });

    applyPlayerNew(world, zappy::PlayerNew::withIdAndTeam(0, "debug"));

    Grid::spawn(world, 10, 10);

    world.system<const Team>("DrawTeamButtons")
        .kind<Render2D>()
        .run([world](flecs::iter &it) {
            auto &state = world.get_mut<ListState>();
            Rectangle btn = { 200, 25, 200, 42 };

            while (it.next()) {
                auto teams = it.field<const Team>(0);
                for (auto i : it) {
                    if (GuiButton(btn, teams[i].name.c_str())) {
                        state.openedTeam = (state.openedTeam == it.entity(i).id()) ? 0 : it.entity(i).id();
                    }
                    btn.x += btn.width + 10;
                }
            }
        });

    world.system<const Player, const zappy::Resources>("DrawPlayerList")
        .kind<Render2D>()
        .run([world](flecs::iter &it) {
            if (auto &state = world.get_mut<ListState>(); state.openedTeam != 0) {
                const Rectangle panel = { 200, 72, 200, 240 };

                DrawRectangleLinesEx(panel, 1.0f, Fade(WHITE, 0.35f));

                Rectangle btn = { panel.x + 8, panel.y + 8, panel.width - 16, 36 };

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
        });

    world.system("DrawInventoryModal")
        .kind<Render2D>()
        .run([world](flecs::iter &) {
            if (auto &state = world.get_mut<ListState>(); state.selectedPlayer != 0 && state.resources.has_value()) {
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
        });

    world.entity("Exit Button")
        .set(Button("Exit"))
        .set(Position2::splat(100).with_y(25))
        .set(OnClick([world](flecs::entity) mutable {
            world.entity<Game>().destruct();
            world.import<Home>().child_of<Scenes>();
        }));

    world.entity("Command Input")
        .set(TextInput(""))
        .set(Position2::bottom_center())
        .set(OnEnter([world](flecs::entity, std::string &text) mutable {
            runCommand(world, text);
            text.clear();
        }));
}
