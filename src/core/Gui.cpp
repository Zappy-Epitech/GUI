#include "Gui.hpp"
#include "Spatial.hpp"
#include "src/core/Raylib.hpp"
#include "src/extern/flecs.h"
#include <cstdio>
#include <cstring>
#include <functional>
#include <raygui.h>
#include <raylib.h>

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

                    if (GuiButton(rect, buttons[i].label.c_str())) {
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
            constexpr int messageFontSize = 24;
            constexpr int lineHeight = 34;
            int row = 0;

            while (it.next()) {
                auto messages = it.field<const ScreenMessage>(0);
                auto colors = it.field<const Color>(1);

                for (auto i : it) {
                    int width = MeasureText(messages[i].value.c_str(), messageFontSize);
                    int x = GetScreenWidth() / 2 - width / 2;
                    int y = GetScreenHeight() - 150 - row * lineHeight;

                    DrawRectangle(x - 10, y - 5, width + 20, messageFontSize + 10, Fade(BLACK, 0.65f));
                    DrawText(messages[i].value.c_str(), x, y, messageFontSize, colors[i]);
                    row++;
                }
            }
        });
}
