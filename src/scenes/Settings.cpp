#include "Settings.hpp"
#include "src/core/Gui.hpp"
#include "src/core/Raylib.hpp"
#include "src/core/Scenes.hpp"
#include "src/core/Settings.hpp"
#include "src/core/Spatial.hpp"
#include "src/extern/flecs.h"
#include "src/scenes/AppScenes.hpp"
#include "src/scenes/Home.hpp"

#include <algorithm>
#include <cmath>
#include <format>
#include <raygui.h>
#include <raylib.h>

namespace {

static void drawCenteredText(const char *text, int y, int fontSize, Color color) {
    const int width = MeasureText(text, fontSize);
    DrawText(text, GetScreenWidth() / 2 - width / 2, y, fontSize, color);
}

} // namespace

/// Registers the settings scene.
SettingsScene::SettingsScene(flecs::world &world) {
    auto module = world.module<SettingsScene>("settings_scene").child_of<AppScenes>();
    (void)module;

    onEnterScene<SettingsScene>(world, "EnterSettings", [](flecs::world &world) {
        world.entity("Settings Back Button")
            .set(Button("Back"))
            .set(Position2::center().add_y(185))
            .set(OnClick([](flecs::entity e) {
                flecs::world world = e.world();
                enterScene<Home>(world);
            }))
            .add<DespawnOnExit>(sceneId<SettingsScene>(world));
    });

    world.system("DrawSettingsPanel")
        .kind<Render2D>()
        .run([world](flecs::iter &) {
            GuiSettings &settings = world.get_mut<GuiSettings>();

            const float panelWidth = 520.0f;
            const float panelHeight = 320.0f;
            const Rectangle panel = {
                static_cast<float>(GetScreenWidth()) * 0.5f - panelWidth * 0.5f,
                static_cast<float>(GetScreenHeight()) * 0.5f - panelHeight * 0.5f - 18.0f,
                panelWidth,
                panelHeight,
            };
            const Rectangle slider = { panel.x + 170.0f, panel.y + 100.0f, panel.width - 265.0f, 28.0f };
            const Rectangle teamNamesCheckbox = { panel.x + 250.0f, panel.y + 154.0f, 24.0f, 24.0f };
            const Rectangle highlightCheckbox = { panel.x + 250.0f, panel.y + 204.0f, 24.0f, 24.0f };

            DrawRectangleRec(panel, Fade(BLACK, 0.55f));
            DrawRectangleLinesEx(panel, 1.0f, Fade(WHITE, 0.4f));
            drawCenteredText("Settings", static_cast<int>(panel.y + 28.0f), 32, WHITE);

            DrawText("Volume", static_cast<int>(panel.x + 46.0f), static_cast<int>(slider.y - 2.0f), 24, WHITE);
            DrawText(std::format("{}%", static_cast<int>(std::round(settings.masterVolume * 100.0f))).c_str(),
                     static_cast<int>(panel.x + panel.width - 80.0f),
                     static_cast<int>(slider.y - 2.0f),
                     24,
                     SKYBLUE);

            GuiSlider(slider, "", "", &settings.masterVolume, 0.0f, 1.0f);
            settings.masterVolume = std::clamp(settings.masterVolume, 0.0f, 1.0f);

            DrawText("Team names", static_cast<int>(panel.x + 46.0f), static_cast<int>(teamNamesCheckbox.y - 2.0f), 24, WHITE);
            GuiCheckBox(teamNamesCheckbox, "", &settings.showTeamNames);

            DrawText("Hover highlight", static_cast<int>(panel.x + 46.0f), static_cast<int>(highlightCheckbox.y - 2.0f), 24, WHITE);
            GuiCheckBox(highlightCheckbox, "", &settings.highlightTeamOnHover);
        })
        .add<InScene>(sceneId<SettingsScene>(world));
}
