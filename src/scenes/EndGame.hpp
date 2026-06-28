/**
 * @file EndGame.hpp
 * @ingroup gui_scenes
 * @brief End-game scene state and the flecs module that registers it.
 */
#pragma once
#include "src/protocol/ZappyProtocol.hpp"
#include <raylib.h>
#include <string>
#include <vector>

namespace flecs {
struct world;
}

/**
 * @brief Stores one winning player snapshot for the end-game scene.
 * @ingroup gui_scenes
 */
struct EndGamePlayerStats {
    std::string name;             ///< Display name of the player.
    int id = 0;                   ///< Server-assigned player id.
    int level = 0;                ///< Elevation level reached by the player.
    zappy::Resources resources = {}; ///< Inventory (food and stone counts) at game end.
    Texture2D skin = {};          ///< Cached Minecraft skin texture used to draw the player's head.
};

/**
 * @brief Stores the winner and player stats shown on the end-game scene.
 * @ingroup gui_scenes
 */
struct EndGameState {
    std::string winner;                      ///< Name of the winning team.
    std::vector<EndGamePlayerStats> players; ///< Snapshots of the winning team's players.
};

/**
 * @brief Registers the end-game scene.
 * @details Declares the EndGameState/EndGamePlayerStats components, creates the
 * EndGameState singleton, spawns the "Back home" button on scene enter and
 * registers the Render2D system that draws the game-over summary.
 * @ingroup gui_scenes
 */
struct EndGame {
    /// Imports the end-game scene.
    EndGame(flecs::world &world);
};
