#pragma once
#include "../extern/flecs.hpp"
#include <cstring>
#include <functional>
#include <string>

struct OnClick : std::function<void(flecs::entity)> {};
struct OnTextUpdate : std::function<void(flecs::entity, std::string &)> {};

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
