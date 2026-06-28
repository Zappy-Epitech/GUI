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

/// Phase for drawing 2D content.
struct Render2D {};

/// Phase for finishing a frame.
struct PostRender {};

/// Returns whether a world position is close enough to be rendered.
bool isWithinRenderDistance(const flecs::world &world, Vector3 position);

/// Registers Raylib rendering systems.
struct Raylib {
    /// Imports the Raylib module.
    Raylib(flecs::world &world);
};
