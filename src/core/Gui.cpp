/**
 * @file Gui.cpp
 * @ingroup gui_core
 * @brief Implements the Gui module: Minecraft-style button drawing, text input and log panel systems.
 */
#include "Gui.hpp"
#include "Spatial.hpp"
#include "src/core/Raylib.hpp"
#include "src/core/Settings.hpp"
#include "src/extern/flecs.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <functional>
#include <raygui.h>
#include <raylib.h>
#include <vector>

/// The font size for GUI text.
static constexpr float fontSize = 40.0f;
/// The horizontal padding for GUI elements.
static constexpr float paddingX = 24.0f * 2.;
/// The vertical padding for GUI elements.
static constexpr float paddingY = 14.0f * 2.;
/// The width of text input fields.
static constexpr float textInputWidth = 320.0f;
/// The height of text input fields.
static constexpr float textInputHeight = 40.0f;

/// Prepares the editable text buffer size.
static int textInputEditBufferSize(TextInput &input) {
    if (input.text.capacity() <= input.text.size() + 1) {
        input.text.reserve((input.text.size() + 1) * 2);
    }
    input.text.resize(input.text.capacity(), '\0');
    return static_cast<int>(input.text.size() + 1);
}

/// Returns text shortened to fit the requested width.
static std::string truncateTextToWidth(const std::string &text, int fontSize, int maxWidth) {
    if (MeasureText(text.c_str(), fontSize) <= maxWidth) {
        return text;
    }

    std::string shortened = text;
    while (!shortened.empty() && MeasureText((shortened + "...").c_str(), fontSize) > maxWidth) {
        shortened.pop_back();
    }
    return shortened + "...";
}

/// Stores one line prepared for log panel rendering.
struct LogPanelMessage {
    std::string text;
    Color color;
};

/// Draws a Minecraft-style beveled button and reports a click.
///
/// Honors raygui's lock state so it behaves like GuiButton when an overlay
/// (such as the settings modal) locks the background widgets.
bool drawMinecraftButton(Rectangle bounds, const char *label, int labelFontSize) {
    const bool locked = GuiIsLocked();
    const bool hovered = !locked && CheckCollisionPointRec(GetMousePosition(), bounds);
    const bool held = hovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    const bool clicked = hovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON);

    const Color outline = { 16, 16, 16, 255 };
    Color faceTop, faceBottom, lightBevel, darkBevel;

    if (locked) {
        faceTop = { 84, 84, 84, 255 };
        faceBottom = { 66, 66, 66, 255 };
        lightBevel = { 104, 104, 104, 255 };
        darkBevel = { 40, 40, 40, 255 };
    } else if (held) {
        faceTop = { 74, 82, 94, 255 };
        faceBottom = { 64, 71, 82, 255 };
        lightBevel = { 40, 44, 50, 255 };
        darkBevel = { 120, 130, 145, 255 };
    } else if (hovered) {
        faceTop = { 120, 132, 148, 255 };
        faceBottom = { 92, 103, 119, 255 };
        lightBevel = { 176, 188, 205, 255 };
        darkBevel = { 44, 49, 57, 255 };
    } else {
        faceTop = { 110, 110, 110, 255 };
        faceBottom = { 84, 84, 84, 255 };
        lightBevel = { 150, 150, 150, 255 };
        darkBevel = { 44, 44, 44, 255 };
    }

    DrawRectangleRec(bounds, outline);

    const Rectangle inner = { bounds.x + 1.0f, bounds.y + 1.0f, bounds.width - 2.0f, bounds.height - 2.0f };
    DrawRectangleGradientV(static_cast<int>(inner.x), static_cast<int>(inner.y), static_cast<int>(inner.width), static_cast<int>(inner.height), faceTop, faceBottom);

    const int bevel = std::max(1, static_cast<int>(inner.height) / 20);
    DrawRectangle(static_cast<int>(inner.x), static_cast<int>(inner.y), static_cast<int>(inner.width), bevel, lightBevel);
    DrawRectangle(static_cast<int>(inner.x), static_cast<int>(inner.y), bevel, static_cast<int>(inner.height), lightBevel);
    DrawRectangle(static_cast<int>(inner.x), static_cast<int>(inner.y + inner.height) - bevel, static_cast<int>(inner.width), bevel, darkBevel);
    DrawRectangle(static_cast<int>(inner.x + inner.width) - bevel, static_cast<int>(inner.y), bevel, static_cast<int>(inner.height), darkBevel);

    const int textWidth = MeasureText(label, labelFontSize);
    const int textX = static_cast<int>(bounds.x + (bounds.width - static_cast<float>(textWidth)) * 0.5f);
    const int textY = static_cast<int>(bounds.y + (bounds.height - static_cast<float>(labelFontSize)) * 0.5f) + (held ? 1 : 0);
    const Color textColor = locked ? Color{ 160, 160, 160, 255 } : (hovered ? Color{ 255, 255, 160, 255 } : Color{ 235, 235, 235, 255 });
    const int shadow = std::max(1, labelFontSize / 12);

    DrawText(label, textX + shadow, textY + shadow, labelFontSize, Fade(BLACK, 0.55f));
    DrawText(label, textX, textY, labelFontSize, textColor);

    return clicked;
}

/// Registers GUI components and render systems.
Gui::Gui(flecs::world &world) {
    world.module<Gui>("gui");
    world.import<Raylib>();
    world.import<Spatial>();

    world.component<Button>()
        .member(flecs::String, "label");

    world.component<TextInput>();
    world.component<OnClick>();
    world.component<OnEnter>();
    world.component<OnTextUpdate>();
    world.component<HasInputActive>();
    world.component<ScreenMessage>();
    world.singleton<HasInputActive>();

    static Sound ButtonSound;

    world.set<HasInputActive>({ false });

    world.system("LoadButtonSound").kind(flecs::OnStart).run([](auto) {
        ButtonSound = LoadSound("./assets/sounds/button.wav");
    });

    world.system<const Position2, const Button, const OnClick>("ButtonSystem")
        .kind<Render2D>()
        .run([](flecs::iter &it) {
            while (it.next()) {
                auto positions = it.field<const Position2>(0);
                auto buttons = it.field<const Button>(1);
                auto onClicks = it.field<const OnClick>(2);

                for (auto i : it) {
                    const float textWidth = buttons[i].label.size() * fontSize * 0.5f;
                    const float width = textWidth + paddingX;
                    const float height = fontSize + paddingY;

                    Rectangle rect = { positions[i].x - width * 0.5f, positions[i].y, width, height };

                    if (drawMinecraftButton(rect, buttons[i].label.c_str(), 28)) {
                        auto e = it.entity(i);
                        onClicks[i](e);
                        PlaySound(ButtonSound);
                    }
                }
            }
        });

    world.system<const Position2, TextInput>("TextInputSystem")
        .with<OnEnter>()
        .optional()
        .with<OnTextUpdate>()
        .optional()
        .kind<Render2D>()
        .run([](flecs::iter &it) {
            bool isEnterButtonReleased = IsKeyPressed(KEY_ENTER);

            while (it.next()) {
                auto positions = it.field<const Position2>(0);
                auto inputs = it.field<TextInput>(1);
                auto onEnters = it.field<const OnEnter>(2);
                auto onTextUpdates = it.field<const OnTextUpdate>(3);

                for (auto i : it) {
                    Rectangle rect = {
                        positions[i].x - textInputWidth * 0.5f,
                        positions[i].y,
                        textInputWidth,
                        textInputHeight,
                    };

                    if (!inputs[i].active) {
                        if (GuiTextBox(rect, inputs[i].text.data(), static_cast<int>(inputs[i].text.size() + 1), false)) {
                            inputs[i].active = true;
                            it.world().set<HasInputActive>({ true });
                        }
                        continue;
                    }

                    std::string previousText = inputs[i].text;
                    int bufferSize = textInputEditBufferSize(inputs[i]);

                    if (GuiTextBox(rect, inputs[i].text.data(), bufferSize, true)) {
                        inputs[i].active = false;
                        it.world().set<HasInputActive>({ false });
                    }

                    inputs[i].text.resize(std::strlen(inputs[i].text.c_str()));

                    if (isEnterButtonReleased && it.is_set(2)) {
                        auto e = it.entity(i);
                        onEnters[i](e, inputs[i].text);
                        PlaySound(ButtonSound);
                    }

                    if (previousText != inputs[i].text) {
                        auto e = it.entity(i);
                        PlaySound(ButtonSound);
                        if (it.is_set(3)) {
                            onTextUpdates[i](e, inputs[i].text);
                        }
                    }
                }
            }
        });

    world.system<const ScreenMessage, const Color>("ScreenMessageRender")
        .kind<Render2D>()
        .run([](flecs::iter &it) {
            const GuiSettings *settings = it.world().try_get<GuiSettings>();

            if (settings != nullptr && !settings->showLogPanel) {
                return;
            }

            constexpr int messageFontSize = 24;
            constexpr int lineHeight = 34;
            constexpr int maxMessages = 8;
            constexpr int panelWidth = 430;
            constexpr int panelPadding = 14;
            constexpr int margin = 24;
            const int panelX = GetScreenWidth() - panelWidth - margin;
            const int panelBottom = GetScreenHeight() - margin;
            std::vector<LogPanelMessage> visibleMessages;
            visibleMessages.reserve(maxMessages);

            while (it.next()) {
                auto messages = it.field<const ScreenMessage>(0);
                auto colors = it.field<const Color>(1);

                for (auto i : it) {
                    if (static_cast<int>(visibleMessages.size()) >= maxMessages) {
                        break;
                    }

                    visibleMessages.push_back(LogPanelMessage{
                        truncateTextToWidth(messages[i].value, messageFontSize, panelWidth - panelPadding * 2),
                        colors[i],
                    });
                }
            }

            if (visibleMessages.empty()) {
                return;
            }

            const int panelHeight = panelPadding * 2 + static_cast<int>(visibleMessages.size()) * lineHeight;
            const int panelY = panelBottom - panelHeight;
            const int textX = panelX + panelPadding;

            DrawRectangle(panelX, panelY, panelWidth, panelHeight, Fade(BLACK, 0.68f));
            DrawRectangleLines(panelX, panelY, panelWidth, panelHeight, Fade(WHITE, 0.32f));

            for (std::size_t i = 0; i < visibleMessages.size(); i++) {
                const int y = panelBottom - panelPadding - messageFontSize - static_cast<int>(i) * lineHeight;

                DrawText(visibleMessages[i].text.c_str(), textX, y, messageFontSize, visibleMessages[i].color);
            }
        });
}
