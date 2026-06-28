/**
 * @file Simulation.hpp
 * @ingroup gui_gameplay
 * @brief Singleton components describing simulation timing and game result.
 */
#pragma once
#include <string>

/**
 * @brief Stores the server time unit (ticks per second).
 * @ingroup gui_gameplay
 */
struct SimulationTime {
    int timeUnit = 0;
};

/**
 * @brief Stores the end-of-game result.
 * @ingroup gui_gameplay
 */
struct GameResult {
    bool finished = false;
    std::string winner;
};
