#pragma once

namespace flecs {
struct world;
}

/// Stores user-configurable GUI settings.
struct GuiSettings {
    float masterVolume = 1.0f;
    bool showTeamNames = true;
    bool highlightTeamOnHover = true;
    bool showLogPanel = true;
};

/// Registers GUI settings state and systems.
struct Settings {
    /// Imports settings components.
    Settings(flecs::world &world);
};
