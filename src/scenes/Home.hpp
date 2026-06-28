/**
 * @file Home.hpp
 * @ingroup gui_scenes
 * @brief flecs module that registers the home/connection scene.
 */
#pragma once

namespace flecs {
struct world;
}

/**
 * @brief Registers the home scene.
 * @details Creates the HomeConnectForm singleton and, on scene enter, spawns the
 * host/port text inputs plus the Play and Settings buttons that connect to a
 * server and enter the game.
 * @ingroup gui_scenes
 */
struct Home {
    /// Imports the home scene.
    Home(flecs::world &world);
};
