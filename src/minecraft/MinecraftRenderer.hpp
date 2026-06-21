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

/// Registers Minecraft skin rendering.
struct MinecraftRenderer {
    /// Imports the renderer module.
    MinecraftRenderer(flecs::world &world);
};
