#pragma once
#include <raylib.h>

namespace flecs {
struct world;
}

/// Stores Minecraft skin render settings.
struct MinecraftSkin {
    float scale = 1.0f;
};

/// Draws the front face of a Minecraft skin head in 2D.
void DrawMinecraftHead(Texture2D skin, Rectangle bounds);

/// Draws a 3D inventory player preview whose head tracks the mouse.
void DrawMinecraftPlayerPreview3D(Texture2D skin, Rectangle bounds, Vector2 mouse);

/// Registers Minecraft skin rendering.
struct MinecraftRenderer {
    /// Imports the renderer module.
    MinecraftRenderer(flecs::world &world);
};
