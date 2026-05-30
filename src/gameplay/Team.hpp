#pragma once

#include <string>

namespace flecs {
struct world;
}

struct Team {
    std::string name;
};

struct BelongsTo {};

struct Teams {
    Teams(flecs::world &world);
};
