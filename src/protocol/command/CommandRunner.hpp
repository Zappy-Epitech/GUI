/**
 * @file CommandRunner.hpp
 * @ingroup gui_protocol
 * @brief Entry point that parses a server command and applies it to the world.
 */
#pragma once
#include <string>

namespace flecs {
struct world;
}

/**
 * @brief Parses one server command and applies its effect to the ECS world.
 * @ingroup gui_protocol
 * @param world The flecs world the resulting event is applied to.
 * @param command A single raw protocol line from the server.
 */
void runCommand(const flecs::world &world, const std::string &command);
