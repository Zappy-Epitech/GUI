#include "Game.hpp"
#include "../../extern/flecs.hpp"
#include "src/modules/Gui.hpp"
#include "src/modules/Raylib.hpp"
#include "src/modules/Spatial.hpp"
#include "src/modules/gameplay/CameraController.hpp"
#include "src/modules/gameplay/GamePlay.hpp"
#include "src/modules/gameplay/Grid.hpp"
#include "src/modules/gameplay/Player.hpp"
#include "src/modules/scenes/Home.hpp"
#include "src/protocol/ZappyProtocol.hpp"
#include "src/protocol/command/CommandRunner.hpp"
#include "src/protocol/command/PlayerCommand.hpp"
#include <format>
#include <raygui.h>
#include <raylib.h>
#include <string>

namespace {

struct PlayerInventoryModal {
    std::string name;
    int level;
    zappy::Resources resources;
};

static void drawResourceRow(Rectangle bounds, const char *label, int amount) {
    GuiLabel(Position2(bounds.x + 16, bounds.y).rect(bounds.width * 0.65f, bounds.height), label);
    GuiLabel(
        Position2(bounds.x + bounds.width - 96, bounds.y).rect(80, bounds.height),
        std::format("{}", amount).c_str());
}

static void drawInventoryModal(const PlayerInventoryModal &player, flecs::entity_t &entity) {
    const Rectangle modal = Position2::center(400, 700).rect(400, 700);

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.55f));

    if (GuiWindowBox(modal, player.name.c_str())) {
        entity = 0;
        return;
    }

    float y = modal.y + 30 + 58;

    using std::pair;

    for (const auto &[label, amount] : {
             pair("level", player.level),
             pair("food", player.resources.food),
             pair("linemate", player.resources.linemate),
             pair("deraumere", player.resources.deraumere),
             pair("sibur", player.resources.sibur),
             pair("mendiane", player.resources.mendiane),
             pair("phiras", player.resources.phiras),
             pair("thystame", player.resources.thystame),
         }) {
        drawResourceRow(
            Rectangle(modal.x + 20, y, modal.width - 40, 58),
            label,
            amount);
        y += 58 + 10;
    }

    if (GuiButton(
            Rectangle(modal.x + 20, modal.y + modal.height - 62, modal.width - 40, 48),
            "Close")) {
        entity = 0;
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

    world.system<const Player, const zappy::Resources>("Draw Player Button")
        .kind<Render2D>()
        .run([](flecs::iter &it) {
            static flecs::entity_t selectedPlayer = 0;
            static std::optional<PlayerInventoryModal> selectedInventory;
            Rectangle buttonBounds = Position2(35, 150).rect(200, 50);

            while (it.next()) {
                auto players = it.field<const Player>(0);
                auto resources = it.field<const zappy::Resources>(1);

                for (auto i : it) {
                    flecs::entity entity = it.entity(i);

                    auto modal = PlayerInventoryModal{ entity.name().c_str(), players[i].level, resources[i] };

                    if (entity.id() == selectedPlayer) {
                        selectedInventory = modal;
                    }

                    if (selectedPlayer == 0 && GuiButton(buttonBounds, entity.name().c_str())) {
                        selectedPlayer = entity.id();
                        selectedInventory = modal;
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
