#pragma once
#include "../extern/flecs.hpp"
#include <cstring>
#include <string>

struct OnClick {};
struct TextUpdated {
    std::string &text;
};

struct Button {
    char *label = strdup("No Label");

    Button() {
    }
    Button(const char *label) : label(strdup(label)) {
    }
};

struct TextInput {
    std::string text;
    bool active = false;

    TextInput() = default;
    TextInput(const char *text) : text(text) {
    }
};

struct Gui {
    Gui(flecs::world &world);
};
