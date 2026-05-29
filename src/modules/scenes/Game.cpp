#include "Game.hpp"
#include "../../extern/flecs.hpp"
#include "src/modules/Gui.hpp"
#include "src/modules/Raylib.hpp"
#include "src/modules/SkinAnimation.hpp"
#include "src/modules/Spatial.hpp"
#include "src/modules/gameplay/CameraController.hpp"
#include "src/modules/gameplay/GamePlay.hpp"
#include "src/modules/gameplay/Grid.hpp"
#include "src/modules/gameplay/Player.hpp"
#include "src/modules/rendering/MinecraftSkinRenderer.hpp"
#include "src/modules/scenes/Home.hpp"
#include "src/protocol/ZappyProtocol.hpp"
#include "src/protocol/command/CommandRunner.hpp"
#include "src/protocol/command/PlayerCommand.hpp"
#include <format>
#include <optional>
#include <raygui.h>
#include <raylib.h>
#include <string>
#include <utility>

namespace {

struct PlayerInventoryModal {
    std::string name;
    zappy::Resources resources;
};

static void drawResourceRow(Rectangle bounds, const char *label, int amount) {
    GuiLabel(Position2(bounds.x + 16, bounds.y).rect(bounds.width * 0.65f, bounds.height), label);
    GuiLabel(
        Position2(bounds.x + bounds.width - 96, bounds.y).rect(80, bounds.height),
        std::format("{}", amount).c_str());
}

static void drawInventoryModal(const PlayerInventoryModal &player, flecs::entity_t &selectedPlayer) {
    const Rectangle modalBounds = Position2::center(400, 700).rect(400, 700);

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.55f));

    if (GuiWindowBox(modalBounds, player.name.c_str())) {
        selectedPlayer = 0;
        return;
    }

    const float contentX = modalBounds.x + 20;
    const float contentWidth = modalBounds.width - 40;
    float rowY = modalBounds.y + 30 + 58;

    for (const auto &[label, amount] : {
             std::pair{ "food", player.resources.food },
             std::pair{ "linemate", player.resources.linemate },
             std::pair{ "deraumere", player.resources.deraumere },
             std::pair{ "sibur", player.resources.sibur },
             std::pair{ "mendiane", player.resources.mendiane },
             std::pair{ "phiras", player.resources.phiras },
             std::pair{ "thystame", player.resources.thystame },
         }) {
        drawResourceRow(
            Rectangle{
                .x = contentX,
                .y = rowY,
                .width = contentWidth,
                .height = 58,
            },
            label,
            amount);
        rowY += 58 + 10;
    }

    if (GuiButton(
            Rectangle{
                .x = contentX,
                .y = modalBounds.y + modalBounds.height - 62,
                .width = contentWidth,
                .height = 48,
            },
            "Close")) {
        selectedPlayer = 0;
    }
}

} // namespace

Game::Game(flecs::world &world) {
    world.module<Game>().child_of<GamePlay>();
    world.entity<CameraController>().enable();

    static Model SteveModel = {};

    zappy::PlayerNew playerNew = zappy::PlayerNew{};
    applyPlayerNew(world, playerNew);

    if (SteveModel.meshCount == 0) {
        SteveModel = LoadModel("./assets/models/steve/scene.gltf");
    }

    Grid::spawn(world, 10, 10);

    world.system<const zappy::Resources>("Draw Player Button")
        .with<Player>()
        .kind<Render2D>()
        .run([](flecs::iter &it) {
            static flecs::entity_t selectedPlayer = 0;
            std::optional<PlayerInventoryModal> selectedInventory;
            Rectangle buttonBounds = {
                .x = 100,
                .y = 150,
                .width = 200,
                .height = 50,
            };

            while (it.next()) {
                auto resources = it.field<const zappy::Resources>(0);

                for (auto i : it) {
                    flecs::entity player = it.entity(i);

                    if (player.id() == selectedPlayer) {
                        selectedInventory = PlayerInventoryModal{
                            .name = player.name().c_str(),
                            .resources = resources[i],
                        };
                    }

                    if (selectedPlayer == 0 && GuiButton(buttonBounds, player.name().c_str())) {
                        selectedPlayer = player.id();
                        selectedInventory = PlayerInventoryModal{
                            .name = player.name().c_str(),
                            .resources = resources[i],
                        };
                    }
                    buttonBounds.y += buttonBounds.height + 10;
                }
            }

            if (selectedPlayer != 0) {
                if (selectedInventory.has_value()) {
                    drawInventoryModal(selectedInventory.value(), selectedPlayer);
                } else {
                    selectedPlayer = 0;
                }
            }
        });

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
