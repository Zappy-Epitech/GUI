#pragma once
#include <string>

/// Stores the server time unit (ticks per second).
struct SimulationTime {
    int timeUnit = 0;
};

/// Stores the end-of-game result.
struct GameResult {
    bool finished = false;
    std::string winner;
};
