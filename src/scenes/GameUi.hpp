/**
 * @file GameUi.hpp
 * @ingroup gui_scenes
 * @brief In-game UI state singleton and the flecs module that draws the HUD.
 */
#pragma once

#include "src/extern/flecs.h"
#include "src/protocol/ZappyProtocol.hpp"

#include <optional>
#include <string>

/**
 * @brief Stores game UI selection state.
 * @ingroup gui_scenes
 */
struct GameUiState {
    flecs::entity_t openedTeam = 0;        ///< Team whose player list panel is open (0 = none).
    flecs::entity_t selectedPlayer = 0;    ///< Player whose inventory modal is shown (0 = none).
    std::optional<zappy::Resources> resources; ///< Cached inventory of the selected player.
    int level = 0;                         ///< Cached level of the selected player.
    std::string playerName;                ///< Cached display name of the selected player.
    float panelPositionX = 0.0f;           ///< X position of the team player-list panel, in pixels.
    float requestedFrequency = 100.0f;     ///< Frequency requested via the slider, in time units.
    int confirmedFrequency = 100;          ///< Last frequency confirmed/sent to the server, in time units.
    bool draggingFrequency = false;        ///< True while the frequency slider is being dragged.
    bool frequencyInitialized = false;     ///< True once the frequency state was seeded from SimulationTime.
    flecs::entity_t hoveredTeam = 0;       ///< Team button currently under the mouse (0 = none).
};

/**
 * @brief Registers game UI observers and render systems.
 * @details Creates the GameUiState singleton, observers that keep the selected
 * player's cached data in sync (OnSet/OnRemove), and Render2D/RenderWorld2D
 * systems drawing broadcast bubbles, team labels, team buttons, the player
 * list, the inventory modal and the frequency slider.
 * @ingroup gui_scenes
 */
struct GameUi {
    /// Imports the game UI module.
    GameUi(flecs::world &world);
};
