#pragma once
#include <cstring>
#include <functional>
#include <string>

namespace flecs {
struct world;
struct entity;
} // namespace flecs

struct OnClick : std::function<void(flecs::entity &)> {};
struct OnEnter : std::function<void(flecs::entity &, std::string &)> {};
struct OnTextUpdate : std::function<void(flecs::entity &, std::string &)> {};
struct HasInputActive {
    bool value = false;
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

struct ScreenMessage {
    std::string value;
};

struct Gui {
    Gui(flecs::world &world);
};
