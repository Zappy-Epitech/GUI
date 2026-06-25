#pragma once
#include <string>

namespace flecs {
struct world;
}

/// Parses and applies one server command.
void runCommand(const flecs::world &world, const std::string &command);
