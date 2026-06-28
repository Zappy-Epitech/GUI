/**
 * @file Egg.hpp
 * @ingroup gui_gameplay
 * @brief Egg components, laying animation API, and the egg visuals module.
 */
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
};

/// Starts the visible player laying sequence.
void startEggLayingAnimation(const flecs::world &world, int playerId);

/**
 * @brief Registers egg gameplay visuals.
 * @ingroup gui_gameplay
 */
struct Eggs {
    /// Imports egg systems.
    Eggs(flecs::world &world);
};
