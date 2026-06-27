#pragma once

namespace flecs {
struct world;
}

/// Tracks whether the settings modal is open.
struct SettingsModalState {
    bool open = false;
};

/// Opens the settings modal.
void openSettingsModal(flecs::world &world);

/// Closes the settings modal.
void closeSettingsModal(flecs::world &world);

/// Toggles the settings modal.
void toggleSettingsModal(flecs::world &world);

/// Registers the settings modal.
struct SettingsModal {
    /// Imports the settings modal.
    SettingsModal(flecs::world &world);
};
