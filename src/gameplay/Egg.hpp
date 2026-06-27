#pragma once

#include "src/core/Spatial.hpp"

namespace flecs {
struct world;
}

/// Stores a server egg id.
struct EggId {
    int value;
};

/// Animates a newly spawned egg with a short pop effect.
struct EggSpawnAnimation {
    Position3 basePosition;
    float elapsed;
    float duration;
};

/// Animates a player during egg laying.
struct EggLayingAnimation {
    Position3 basePosition;
    float elapsed;
    float duration;
    bool previewStarted;
};

/// Starts the visible player laying sequence.
void startEggLayingAnimation(const flecs::world &world, int playerId);

/// Registers egg gameplay visuals.
struct Eggs {
    /// Imports egg systems.
    Eggs(flecs::world &world);
};
