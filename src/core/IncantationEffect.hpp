#pragma once

#include "src/extern/flecs.h"
#include <vector>

/// Stores the visual state of an incantation on a tile.
struct IncantationEffect {
    int x = 0;
    int y = 0;
    int level = 1;
    float elapsed = 0.0f;
    float duration = 3.0f;
    bool finished = false;
    bool success = false;
    float resultLifetime = 0.0f;
};

/// Links a player to the incantation visual it participates in.
struct IncantationParticipant {
    flecs::entity_t incantation = 0;
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

/// Registers incantation visual components and systems.
struct IncantationEffects {
    /// Imports the incantation visual module.
    IncantationEffects(flecs::world &world);
};
