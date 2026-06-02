#pragma once

#include "src/extern/flecs.h"
#include "src/protocol/ZappyProtocol.hpp"

#include <optional>
#include <string>

/// Stores game UI selection state.
struct GameUiState {
    flecs::entity_t openedTeam = 0;
    flecs::entity_t selectedPlayer = 0;
    std::optional<zappy::Resources> resources;
    int level = 0;
    std::string playerName;
    float panelPositionX = 0.0f;
};

/// Registers game UI observers and render systems.
struct GameUi {
    /// Imports the game UI module.
    GameUi(flecs::world &world);
};
