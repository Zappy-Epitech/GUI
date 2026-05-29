#include "Gui.hpp"
#include "../extern/flecs.hpp"
#include "Spatial.hpp"
#include "src/modules/Raylib.hpp"
#include <cstdio>
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
    world.component<OnEnter>();
    world.component<OnTextUpdate>();
    world.component<HasInputActive>();
    world.singleton<HasInputActive>();

    static Sound ButtonSound;

    world.set<HasInputActive>({ false });

    world.system("Load Button Sound").kind(flecs::OnStart).run([](auto) {
        ButtonSound = LoadSound("./assets/sounds/button.wav");
    });

    world.system<const Position2, const Button, const OnClick>("Button System")
        .kind<Render2D>()
        .immediate()
        .run([](flecs::iter &it) {
            while (it.next()) {
                auto positions = it.field<const Position2>(0);
                auto buttons = it.field<const Button>(1);
                auto onClicks = it.field<const OnClick>(2);

                for (auto i : it) {
                    const float textWidth = strlen(buttons[i].label) * fontSize * 0.5f;
                    const float width = textWidth + paddingX;
                    const float height = fontSize + paddingY;

                    Rectangle rect = { positions[i].x - width * 0.5f, positions[i].y, width, height };

                    if (GuiButton(rect, buttons[i].label)) {
                        auto e = it.entity(i);
                        onClicks[i](e);
                        PlaySound(ButtonSound);
                    }
                }
            }
        });

    world.system<Position2, TextInput>("Text Input System")
        .kind<Render2D>()
        .run([](flecs::iter &it) {
            bool isEnterButtonReleased = IsKeyPressed(KEY_ENTER);

            while (it.next()) {
                auto positions = it.field<Position2>(0);
                auto inputs = it.field<TextInput>(1);

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

                    if (isEnterButtonReleased) {
                        auto e = it.entity(i);
                        const OnEnter *onEnter = e.try_get<OnEnter>();
                        if (onEnter) {
                            (*onEnter)(e, inputs[i].text);
                        }
                    }

                    if (previousText != inputs[i].text) {
                        auto e = it.entity(i);
                        const OnTextUpdate *onTextUpdate = e.try_get<OnTextUpdate>();
                        if (onTextUpdate) {
                            (*onTextUpdate)(e, inputs[i].text);
                        }
                    }
                }
            }
        });
}
