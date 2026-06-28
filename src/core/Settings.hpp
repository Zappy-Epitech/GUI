/**
 * @file Settings.hpp
 * @ingroup gui_core
 * @brief User-configurable GUI settings component and its module.
 */
#pragma once

namespace flecs {
struct world;
}

/// Stores user-configurable GUI settings.
struct GuiSettings {
    float masterVolume = 1.0f;
    float renderDistance = 35.0f;
    bool showTeamNames = true;
    bool highlightTeamOnHover = true;
    bool showLogPanel = true;
};

/**
 * @brief ECS module managing user-configurable GUI settings.
 * @details Registers the GuiSettings component as a singleton (with reflected
 * members) and adds the ApplyMasterVolume system that pushes the master volume
 * to the audio device each frame.
 * @ingroup gui_core
 */
struct Settings {
    /// Imports settings components.
    Settings(flecs::world &world);
};
