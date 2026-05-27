#pragma once
#include "../extern/flecs.hpp"
#include <cstring>

struct OnClick {};

struct Button {
    char *label = strdup("No Label");

    Button() {
    }
    Button(const char *label) : label(strdup(label)) {
    }
};
struct Gui {
    Gui(flecs::world &world);
};
