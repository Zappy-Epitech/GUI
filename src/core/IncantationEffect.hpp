/**
 * @file IncantationEffect.hpp
 * @ingroup gui_core
 * @brief Incantation visual effect components, lifecycle helpers and module.
 */
#pragma once

#include "src/extern/flecs.h"
#include <vector>

/// Stores the visual state of an incantation on a tile.
struct IncantationEffect {
    int x = 0;                  ///< Tile X coordinate of the incantation.
    int y = 0;                  ///< Tile Y coordinate of the incantation.
    int level = 1;              ///< Incantation level being attempted.
    float elapsed = 0.0f;       ///< Seconds elapsed since the effect started.
    float duration = 3.0f;      ///< Total active duration in seconds.
    bool finished = false;      ///< True once the incantation has resolved.
    bool success = false;       ///< Outcome shown when finished (success or failure).
    float resultLifetime = 0.0f; ///< Seconds the success/failure burst remains visible.
};

/// Links a player to the incantation visual it participates in.
struct IncantationParticipant {
    flecs::entity_t incantation = 0; ///< Entity id of the IncantationEffect the player joined.
};

/// Starts an incantation visual effect and marks participating players.
flecs::entity startIncantationEffect(
    const flecs::world &world,
    int x,
    int y,
    int level,
    const std::vector<int> &playerIds);

/// Finishes an incantation visual effect and switches it to success/failure feedback.
void finishIncantationEffect(const flecs::world &world, int x, int y, bool success);

/**
 * @brief ECS module rendering incantation visual effects.
 * @details Registers the IncantationEffect and IncantationParticipant components
 * and adds systems that advance each effect's timers (destroying it and clearing
 * participants when finished) and draw the 3D tile rings, particles, result
 * bursts and participant markers within render distance.
 * @ingroup gui_core
 */
struct IncantationEffects {
    /// Imports the incantation visual module.
    IncantationEffects(flecs::world &world);
};
