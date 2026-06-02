#pragma once
#include <functional>
#include <string>

namespace flecs {
struct world;
struct entity;
} // namespace flecs

/// Stores a button click callback.
struct OnClick : std::function<void(flecs::entity &)> {};

/// Stores a text submit callback.
struct OnEnter : std::function<void(flecs::entity &, std::string &)> {};

/// Stores a text change callback.
struct OnTextUpdate : std::function<void(flecs::entity &, std::string &)> {};

/// Tracks whether an input is active.
struct HasInputActive {
    bool value = false;
};

/// Stores a button label.
struct Button {
    std::string label = "No Label";

    /// Creates a default button label.
    Button() {
    }

    /// Creates a button with a label.
    Button(const char *label) : label(label) {
    }
};

/// Stores editable text input state.
struct TextInput {
    std::string text;
    bool active = false;

    /// Creates an empty text input.
    TextInput() = default;

    /// Creates a text input with initial text.
    TextInput(const char *text) : text(text) {
    }
};

/// Stores a temporary screen message.
struct ScreenMessage {
    std::string value;
};

/// Registers GUI components and systems.
struct Gui {
    /// Imports the GUI module.
    Gui(flecs::world &world);
};
