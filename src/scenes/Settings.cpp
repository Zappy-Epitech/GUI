#include "Settings.hpp"
#include "src/core/Raylib.hpp"
#include "src/core/Settings.hpp"
#include "src/extern/flecs.h"
#include "src/scenes/AppScenes.hpp"

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

/// Opens the settings modal.
void openSettingsModal(flecs::world &world) {
    world.get_mut<SettingsModalState>().open = true;
}

/// Closes the settings modal.
void closeSettingsModal(flecs::world &world) {
    world.get_mut<SettingsModalState>().open = false;
}

/// Toggles the settings modal.
void toggleSettingsModal(flecs::world &world) {
    SettingsModalState &state = world.get_mut<SettingsModalState>();
    state.open = !state.open;
}

/// Registers the settings modal as a scene-independent overlay.
SettingsModal::SettingsModal(flecs::world &world) {
    world.module<SettingsModal>("settings_modal").child_of<AppScenes>();

    world.singleton<SettingsModalState>().set<SettingsModalState>({});

    // Locks every other GUI control while the modal is open so clicks cannot
    // leak through the dimmed background to the widgets drawn underneath.
    world.system("LockGuiForSettingsModal")
        .kind(flecs::PreUpdate)
        .run([world](flecs::iter &) {
            if (world.get<SettingsModalState>().open) {
                GuiLock();
            }
        });

    world.system("DrawSettingsModal")
        .kind<Render2D>()
        .run([world](flecs::iter &) {
            SettingsModalState &modal = world.get_mut<SettingsModalState>();
            if (!modal.open) {
                return;
            }

            GuiUnlock();

            GuiSettings &settings = world.get_mut<GuiSettings>();

            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.55f));

            const float panelWidth = 520.0f;
            const float panelHeight = 380.0f;
            const Rectangle panel = {
                static_cast<float>(GetScreenWidth()) * 0.5f - panelWidth * 0.5f,
                static_cast<float>(GetScreenHeight()) * 0.5f - panelHeight * 0.5f,
                panelWidth,
                panelHeight,
            };
            const Rectangle slider = { panel.x + 170.0f, panel.y + 100.0f, panel.width - 265.0f, 28.0f };
            const Rectangle teamNamesCheckbox = { panel.x + 250.0f, panel.y + 154.0f, 24.0f, 24.0f };
            const Rectangle highlightCheckbox = { panel.x + 250.0f, panel.y + 204.0f, 24.0f, 24.0f };
            const Rectangle logPanelCheckbox = { panel.x + 250.0f, panel.y + 254.0f, 24.0f, 24.0f };
            const Rectangle closeButton = { panel.x + panel.width - 38.0f, panel.y + 12.0f, 26.0f, 26.0f };

            DrawRectangleRec(panel, Fade(BLACK, 0.85f));
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

            DrawText("Log panel", static_cast<int>(panel.x + 46.0f), static_cast<int>(logPanelCheckbox.y - 2.0f), 24, WHITE);
            GuiCheckBox(logPanelCheckbox, "", &settings.showLogPanel);

            if (GuiButton(closeButton, "x") || IsKeyPressed(KEY_ESCAPE)) {
                modal.open = false;
            }

            GuiUnlock();
        });
}
