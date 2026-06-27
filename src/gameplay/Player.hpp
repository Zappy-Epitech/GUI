#pragma once
#include <cstddef>
#include <string>

namespace flecs {
struct world;
}

/// Stores player state.
struct Player {
    int level;
};

/// Stores a server player id.
struct PlayerId {
    int value;
};

/// Stores the index of the skin assigned to a player.
struct PlayerSkin {
    std::size_t index;
};

/// Marks a player in incantation.
struct Incantating {};

/// Stores the current broadcast bubble shown over a player.
struct PlayerBroadcastBubble {
    std::string message;
    float remaining = 0.0f;
};

/// Animates a player pushing both arms forward during expulsion.
struct PlayerExpelAnimation {
    float elapsed;
    float duration;
};

/// Starts the visible player expulsion sequence.
void startPlayerExpelAnimation(const flecs::world &world, int playerId);

/// Registers player gameplay visuals.
struct Players {
    /// Imports player systems.
    Players(flecs::world &world);
};
