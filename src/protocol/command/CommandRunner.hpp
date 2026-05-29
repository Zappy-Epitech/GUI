#pragma once
#include <string>

namespace flecs {
struct world;
}

void runCommand(flecs::world &world, std::string &command);
