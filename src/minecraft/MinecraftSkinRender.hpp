/**
 * @file MinecraftSkinRender.hpp
 * @ingroup gui_minecraft
 * @brief Standalone helpers to draw Minecraft skins: 2D head, full 3D player, and inventory preview.
 */
#pragma once
#include "src/minecraft/MinecraftAnimation.hpp"
#include <raylib.h>

/// Draws the front face of a Minecraft skin head in 2D.
void DrawMinecraftHead(Texture2D skin, Rectangle bounds);

/// Draws a full 3D Minecraft player.
void DrawMinecraftPlayer(Texture2D skin, Vector3 position, float scale, float yaw, const SkinPose &pose, Color tint);

/// Draws a 3D inventory player preview whose head tracks the mouse.
void DrawMinecraftPlayerPreview3D(Texture2D skin, Rectangle bounds, Vector2 mouse);
