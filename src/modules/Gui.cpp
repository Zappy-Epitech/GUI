#include "Gui.hpp"
#include "../extern/flecs.hpp"
#include "Spatial.hpp"
#include <cstring>
#include <raylib.h>

static constexpr float fontSize = 40.0f;
static constexpr float paddingX = 24.0f * 2.;
static constexpr float paddingY = 14.0f * 2.;

Gui::Gui(flecs::world &world) {
    world.module<Gui>();
    world.import<Spatial>();
    world.component<Button>().member(flecs::String, "label");
    world.component<OnClick>();

    world.system<Position2, Button>("Button System")
        .kind(flecs::OnUpdate)
        .run([](flecs::iter &it) {
            Vector2 position = GetMousePosition();
            bool mouseReleased = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
            bool mousePressed = IsMouseButtonDown(MOUSE_BUTTON_LEFT);

            while (it.next()) {
                auto positions = it.field<Position2>(0);
                auto buttons = it.field<Button>(1);

                for (auto i : it) {
                    const float textWidth = strlen(buttons[i].label) * fontSize * 0.5f;
                    const float width = textWidth + paddingX;
                    const float height = fontSize + paddingY;
                    Color color = RAYWHITE;

                    Rectangle rect = { positions[i].x - width * 0.5f, positions[i].y, width, height };

                    if (CheckCollisionPointRec(position, rect)) {
                        if (mouseReleased) {
                            it.entity(i).emit<OnClick>();
                        }
                        if (!mousePressed) {
                            color = RED;
                        }
                    }

                    DrawRectangleRec(rect, color);

                    DrawText(
                        buttons[i].label,
                        positions[i].x - textWidth * 0.5f,
                        positions[i].y + (height - fontSize) * 0.5f,
                        fontSize,
                        BLACK);
                }
            }
        });
}
