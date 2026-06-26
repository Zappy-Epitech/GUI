#pragma once
#include <cstddef>
#include <string>

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
