/**
 * @file Raylib.hpp
 * @ingroup gui_core
 * @brief Raylib render pipeline: render phase tags, render-distance helper and module.
 */
#pragma once
namespace flecs {
struct world;
}

struct Vector3;

/// Marks an entity rendered as a cube.
struct Cube {};

/// Phase for starting 3D rendering.
struct Render3D {};

/// Phase for drawing 3D content.
struct Draw3D {};

/// Phase for closing 3D rendering.
struct CloseRender3D {};

/// Phase for screen-space overlays attached to 3D world positions.
struct RenderWorld2D {};

/// Phase for drawing 2D content.
struct Render2D {};

/// Phase for finishing a frame.
struct PostRender {};

/// Returns whether a world position is close enough to be rendered.
bool isWithinRenderDistance(const flecs::world &world, Vector3 position);

/**
 * @brief ECS module wiring Raylib into the render pipeline.
 * @details Registers the Color, Cube, Model and Texture2D components and the
 * ordered render phases (Render3D, Draw3D, CloseRender3D, RenderWorld2D,
 * Render2D, PostRender). Adds systems that open the window and audio device,
 * begin/end each frame and 3D mode, and draw cubes and (rotated) models within
 * render distance.
 * @ingroup gui_core
 */
struct Raylib {
    /// Imports the Raylib module.
    Raylib(flecs::world &world);
};
