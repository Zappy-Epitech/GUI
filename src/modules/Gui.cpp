#include "Gui.hpp"
#include "../extern/flecs.hpp"
#include "Spatial.hpp"
#include "src/modules/Postframe.hpp"
#include "src/modules/Raylib.hpp"
#include <cstring>
#include <raygui.h>
#include <raylib.h>

static constexpr float fontSize = 40.0f;
static constexpr float paddingX = 24.0f * 2.;
static constexpr float paddingY = 14.0f * 2.;
static constexpr float textInputWidth = 320.0f;
static constexpr float textInputHeight = 40.0f;

static int textInputEditBufferSize(TextInput &input) {
    if (input.text.capacity() <= input.text.size() + 1) {
        input.text.reserve((input.text.size() + 1) * 2);
    }
    input.text.resize(input.text.capacity(), '\0');
    return static_cast<int>(input.text.size() + 1);
}

Gui::Gui(flecs::world &world) {
    world.module<Gui>();
    world.import<Raylib>();
    world.import<Spatial>();
    world.component<Button>().member(flecs::String, "label");
    world.component<TextInput>();
    world.component<OnClick>();
    world.component<OnTextUpdate>();

    world.system<Position2, Button, OnClick>("Button System")
        .kind(flecs::OnUpdate)
        .run([](flecs::iter &it) {
            while (it.next()) {
                auto positions = it.field<Position2>(0);
                auto buttons = it.field<Button>(1);
                auto onClicks = it.field<OnClick>(2);

                for (auto i : it) {
                    const float textWidth = strlen(buttons[i].label) * fontSize * 0.5f;
                    const float width = textWidth + paddingX;
                    const float height = fontSize + paddingY;

                    Rectangle rect = { positions[i].x - width * 0.5f, positions[i].y, width, height };

                    if (GuiButton(rect, buttons[i].label)) {
                        later([func = onClicks[i], entity = it.entity(i)](flecs::world &) {
                            func(entity);
                        });
                    }
                }
            }
        });

    world.system<Position2, TextInput, OnTextUpdate>("Text Input System")
        .kind(flecs::OnUpdate)
        .run([](flecs::iter &it) {
            while (it.next()) {
                auto positions = it.field<Position2>(0);
                auto inputs = it.field<TextInput>(1);
                auto onTextUpdates = it.field<OnTextUpdate>(2);

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
                        }
                        continue;
                    }

                    std::string previousText = inputs[i].text;
                    int bufferSize = textInputEditBufferSize(inputs[i]);

                    if (GuiTextBox(rect, inputs[i].text.data(), bufferSize, true)) {
                        inputs[i].active = false;
                    }
                    inputs[i].text.resize(std::strlen(inputs[i].text.c_str()));

                    if (previousText != inputs[i].text) {
                        later([func = onTextUpdates[i], entity = it.entity(i), &text = inputs[i].text](flecs::world &) {
                            func(entity, text);
                        });
                    }
                }
            }
        });
}
