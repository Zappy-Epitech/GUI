/**
 * @file MinecraftRenderer.hpp
 * @ingroup gui_minecraft
 * @brief flecs module and entry points for drawing animated 3D Minecraft-style players.
 */
#pragma once
#include <raylib.h>

namespace flecs {
struct world;
}

/// Stores Minecraft skin render settings.
struct MinecraftSkin {
    float scale = 1.0f; ///< World-space height scale of the rendered player model.
};

/// Draws the front face of a Minecraft skin head in 2D.
void DrawMinecraftHead(Texture2D skin, Rectangle bounds);

/// Draws a 3D inventory player preview whose head tracks the mouse.
void DrawMinecraftPlayerPreview3D(Texture2D skin, Rectangle bounds, Vector2 mouse);

/**
 * @brief flecs module that registers the Minecraft skin rendering system.
 * @ingroup gui_minecraft
 * @details Registers the MinecraftSkin component and the @c SkinRender Draw3D
 * system which gathers visible players, applies same-tile crowd layout, and
 * draws each animated skin (with an optional team-hover highlight).
 */
struct MinecraftRenderer {
    /// Imports the renderer module.
    MinecraftRenderer(flecs::world &world);
};
