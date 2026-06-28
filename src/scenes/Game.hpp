/**
 * @file Game.hpp
 * @ingroup gui_scenes
 * @brief flecs module that registers the main in-game scene.
 */
#pragma once

namespace flecs {
struct world;
}

/**
 * @brief Registers the game scene.
 * @details Declares the gameplay components (Player, PlayerId, PlayerSkin,
 * Incantating, PlayerBroadcastBubble) and imports the GameUi and GameLifecycle
 * sub-modules.
 * @ingroup gui_scenes
 */
struct Game {
    /// Imports the game scene.
    Game(flecs::world &world);
};
