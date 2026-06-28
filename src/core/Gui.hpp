/**
 * @file Gui.hpp
 * @ingroup gui_core
 * @brief 2D GUI building blocks: buttons, text inputs, callbacks, screen messages and module.
 */
#pragma once
#include <functional>
#include <raylib.h>
#include <string>

namespace flecs {
struct world;
struct entity;
} // namespace flecs

/// Draws a Minecraft-style button and returns true when it is clicked.
bool drawMinecraftButton(Rectangle bounds, const char *label, int labelFontSize);

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

/**
 * @brief ECS module providing the 2D GUI widgets and their render systems.
 * @details Registers the Button, TextInput, OnClick, OnEnter, OnTextUpdate,
 * HasInputActive and ScreenMessage components. Adds systems that load the button
 * click sound and, in the Render2D phase, draw and handle buttons, text inputs
 * (with enter/change callbacks), and the on-screen log message panel.
 * @ingroup gui_core
 */
struct Gui {
    /// Imports the GUI module.
    Gui(flecs::world &world);
};
