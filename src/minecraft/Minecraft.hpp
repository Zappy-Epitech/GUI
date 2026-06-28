/**
 * @file Minecraft.hpp
 * @ingroup gui_minecraft
 * @brief Top-level flecs module aggregating the Minecraft-style player rendering subsystems.
 */
#pragma once

namespace flecs {
struct world;
}

/**
 * @brief Top-level flecs module for Minecraft-style player rendering.
 * @ingroup gui_minecraft
 * @details Imports the child modules that register skin animation (limb
 * keyframe tracks, pose update system) and skin rendering (3D player draw
 * system with same-tile crowd layout).
 */
struct Minecraft {
    /// Imports the MinecraftAnimation and MinecraftRenderer child modules.
    Minecraft(flecs::world &world);
};
