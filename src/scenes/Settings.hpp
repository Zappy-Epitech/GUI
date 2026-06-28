/**
 * @file Settings.hpp
 * @ingroup gui_scenes
 * @brief Settings modal overlay state, control helpers and its flecs module.
 */
#pragma once

namespace flecs {
struct world;
}

/**
 * @brief Tracks whether the settings modal is open.
 * @ingroup gui_scenes
 */
struct SettingsModalState {
    bool open = false; ///< True while the settings overlay is visible.
};

/// Opens the settings modal.
void openSettingsModal(flecs::world &world);

/// Closes the settings modal.
void closeSettingsModal(flecs::world &world);

/// Toggles the settings modal.
void toggleSettingsModal(flecs::world &world);

/**
 * @brief Registers the settings modal.
 * @details Creates the SettingsModalState singleton, a PreUpdate system that
 * locks the GUI while the modal is open, and a Render2D system that draws the
 * dimmed overlay with the volume, render-distance, team-names, hover-highlight
 * and log-panel controls bound to GuiSettings.
 * @ingroup gui_scenes
 */
struct SettingsModal {
    /// Imports the settings modal.
    SettingsModal(flecs::world &world);
};
