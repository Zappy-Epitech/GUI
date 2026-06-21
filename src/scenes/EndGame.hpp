#pragma once
#include "src/protocol/ZappyProtocol.hpp"
#include <raylib.h>
#include <string>
#include <vector>

namespace flecs {
struct world;
}

/// Stores one winning player snapshot for the end-game scene.
struct EndGamePlayerStats {
    std::string name;
    int id = 0;
    int level = 0;
    zappy::Resources resources = {};
    Texture2D skin = {};
};

/// Stores the winner and player stats shown on the end-game scene.
struct EndGameState {
    std::string winner;
    std::vector<EndGamePlayerStats> players;
};

/// Registers the end-game scene.
struct EndGame {
    /// Imports the end-game scene.
    EndGame(flecs::world &world);
};
