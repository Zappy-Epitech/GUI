#include "GameUi.hpp"
#include "src/core/Gui.hpp"
#include "src/core/Raylib.hpp"
#include "src/core/Scenes.hpp"
#include "src/core/Settings.hpp"
#include "src/core/Spatial.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/CameraController.hpp"
#include "src/gameplay/GameAssets.hpp"
#include "src/gameplay/Player.hpp"
#include "src/gameplay/Simulation.hpp"
#include "src/gameplay/Team.hpp"
#include "src/minecraft/MinecraftRenderer.hpp"
#include "src/network/NetworkModule.hpp"
#include "src/scenes/Game.hpp"

#include <algorithm>
#include <cmath>
#include <format>
#include <raygui.h>
#include <raylib.h>
#include <sstream>
#include <tuple>
#include <vector>

namespace {

constexpr int minFrequency = 1;
constexpr int maxFrequency = 1000;

static int normalizedFrequency(float value) {
    return std::clamp(static_cast<int>(std::round(value)), minFrequency, maxFrequency);
}

static void initializeFrequencyState(const flecs::world &world, GameUiState &state) {
    if (state.frequencyInitialized) {
        return;
    }

    const SimulationTime *time = world.try_get<SimulationTime>();
    const int frequency = std::clamp(time ? time->timeUnit : state.confirmedFrequency, minFrequency, maxFrequency);
    state.confirmedFrequency = frequency;
    state.requestedFrequency = static_cast<float>(frequency);
    state.frequencyInitialized = true;
}

static std::array<int, 7> resourceAmounts(const zappy::Resources &resources) {
    return {
        resources.food,
        resources.linemate,
        resources.deraumere,
        resources.sibur,
        resources.mendiane,
        resources.phiras,
        resources.thystame,
    };
}

static void drawItemCount(int amount, Rectangle slot, float scale) {
    const std::string text = std::format("{}", amount);
    const int fontSize = std::max(12, static_cast<int>(8.0f * scale));
    const int width = MeasureText(text.c_str(), fontSize);
    const int x = static_cast<int>(slot.x + slot.width - static_cast<float>(width) - 1.0f * scale);
    const int y = static_cast<int>(slot.y + slot.height - static_cast<float>(fontSize) - 1.0f * scale);

    DrawText(text.c_str(), x + 1, y + 1, fontSize, BLACK);
    DrawText(text.c_str(), x, y, fontSize, WHITE);
}

static void drawMinecraftInventory(const GameAssets &assets, const zappy::Resources &resources, Rectangle bounds) {
    constexpr Rectangle source = { 0.0f, 0.0f, 176.0f, 166.0f };
    constexpr float slotSize = 18.0f;
    constexpr float iconSize = 16.0f;
    constexpr float hotbarX = 7.0f;
    constexpr float hotbarY = 141.0f;

    const float scale = bounds.width / source.width;
    DrawTexturePro(assets.inventoryGuiTexture, source, bounds, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);

    const auto amounts = resourceAmounts(resources);
    for (std::size_t i = 0; i < amounts.size(); ++i) {
        const Rectangle slot = {
            bounds.x + (hotbarX + static_cast<float>(i) * slotSize) * scale,
            bounds.y + hotbarY * scale,
            slotSize * scale,
            slotSize * scale,
        };
        const Rectangle icon = {
            slot.x + scale,
            slot.y + scale,
            iconSize * scale,
            iconSize * scale,
        };

        DrawTexturePro(assets.resourceIconTextures[i],
                       Rectangle{ 0.0f, 0.0f, 16.0f, 16.0f },
                       icon,
                       Vector2{ 0.0f, 0.0f },
                       0.0f,
                       WHITE);
        drawItemCount(amounts[i], slot, scale);
    }
}

static float vectorDot(Vector3 left, Vector3 right) {
    return left.x * right.x + left.y * right.y + left.z * right.z;
}

static Vector3 vectorSubtract(Vector3 left, Vector3 right) {
    return Vector3{ left.x - right.x, left.y - right.y, left.z - right.z };
}

static float vectorLength(Vector3 value) {
    return std::sqrt(value.x * value.x + value.y * value.y + value.z * value.z);
}

static float playerLabelScale(Vector3 playerPosition) {
    const float distance = vectorLength(vectorSubtract(playerPosition, CameraController::camera().position));

    return std::clamp(10.0f / std::max(distance, 1.0f), 0.9f, 1.65f);
}

static std::vector<std::string> wrapBubbleText(const std::string &text, int fontSize, int maxWidth) {
    std::vector<std::string> lines;
    std::istringstream words(text);
    std::string word;
    std::string line;

    while (words >> word) {
        const std::string candidate = line.empty() ? word : line + " " + word;
        if (!line.empty() && MeasureText(candidate.c_str(), fontSize) > maxWidth) {
            lines.push_back(line);
            line = word;
        } else {
            line = candidate;
        }
    }

    if (!line.empty()) {
        lines.push_back(line);
    }
    if (lines.empty()) {
        lines.push_back("");
    }
    return lines;
}

static void drawBroadcastBubble(const flecs::world &world, const std::string &message, Vector3 playerPosition) {
    if (!isWithinRenderDistance(world, playerPosition)) {
        return;
    }

    const Camera &camera = CameraController::camera();
    const Vector3 cameraForward = vectorSubtract(camera.target, camera.position);
    const Vector3 toBubble = vectorSubtract(playerPosition, camera.position);

    if (vectorDot(cameraForward, toBubble) <= 0.0f) {
        return;
    }

    const float scale = playerLabelScale(playerPosition);
    const Vector2 head = GetWorldToScreen(Vector3{ playerPosition.x, playerPosition.y + 0.82f, playerPosition.z }, camera);
    const int fontSize = static_cast<int>(std::round(18.0f * scale));
    const int maxTextWidth = static_cast<int>(320.0f * scale);
    const int paddingX = static_cast<int>(12.0f * scale);
    const int paddingY = static_cast<int>(8.0f * scale);
    const int lineGap = static_cast<int>(4.0f * scale);
    const std::vector<std::string> lines = wrapBubbleText(message, fontSize, maxTextWidth);

    int textWidth = 0;
    for (const std::string &line : lines) {
        textWidth = std::max(textWidth, MeasureText(line.c_str(), fontSize));
    }

    const float width = static_cast<float>(textWidth + paddingX * 2);
    const float height = static_cast<float>(static_cast<int>(lines.size()) * fontSize + (static_cast<int>(lines.size()) - 1) * lineGap + paddingY * 2);
    const Rectangle bubble = {
        head.x - width * 0.5f,
        head.y - height - 26.0f * scale,
        width,
        height,
    };

    DrawRectangleRounded(bubble, 0.22f, 8, Fade(BLACK, 0.72f));
    DrawRectangleRoundedLines(bubble, 0.22f, 8, Fade(WHITE, 0.86f));

    float y = bubble.y + static_cast<float>(paddingY);
    for (const std::string &line : lines) {
        const int lineWidth = MeasureText(line.c_str(), fontSize);
        DrawText(line.c_str(), static_cast<int>(bubble.x + (bubble.width - static_cast<float>(lineWidth)) * 0.5f), static_cast<int>(y), fontSize, WHITE);
        y += static_cast<float>(fontSize + lineGap);
    }
}

static bool isInFrontOfCamera(Vector3 worldPosition) {
    const Camera &camera = CameraController::camera();
    const Vector3 cameraForward = vectorSubtract(camera.target, camera.position);
    const Vector3 toLabel = vectorSubtract(worldPosition, camera.position);

    return vectorDot(cameraForward, toLabel) > 0.0f;
}

static void drawPlayerLabel(const flecs::world &world, const std::string &text, Vector3 playerPosition, int baseFontSize, Color color) {
    if (!isWithinRenderDistance(world, playerPosition)) {
        return;
    }

    if (!isInFrontOfCamera(playerPosition)) {
        return;
    }

    const float scale = playerLabelScale(playerPosition);
    const Vector2 anchor = GetWorldToScreen(Vector3{ playerPosition.x, playerPosition.y + 0.82f, playerPosition.z }, CameraController::camera());
    const int fontSize = static_cast<int>(std::round(static_cast<float>(baseFontSize) * scale));
    const int width = MeasureText(text.c_str(), fontSize);
    const int x = static_cast<int>(anchor.x - static_cast<float>(width) * 0.5f);
    const int y = static_cast<int>(anchor.y - static_cast<float>(fontSize) * 0.5f + 10.0f * scale);

    DrawText(text.c_str(), x + 1, y + 1, fontSize, BLACK);
    DrawText(text.c_str(), x, y, fontSize, color);
}

} // namespace

/// Registers game UI observers and render systems.
GameUi::GameUi(flecs::world &world) {
    auto module = world.module<GameUi>("ui").child_of<Game>();

    world.singleton<GameUiState>().set<GameUiState>({}).child_of(module);

    world.observer<const Player, const zappy::Resources>("OnSetPlayerResources")
        .event(flecs::OnSet)
        .each([world](flecs::entity e, const Player &p, const zappy::Resources &r) {
            auto &state = world.get_mut<GameUiState>();
            if (e.id() == state.selectedPlayer) {
                state.playerName = e.name().c_str();
                state.level = p.level;
                state.resources = r;
            }
        });

    world.observer<const Player>("OnRemovePlayer")
        .event(flecs::OnRemove)
        .each([world](flecs::entity e, const Player &) {
            auto &state = world.get_mut<GameUiState>();
            if (e.id() == state.selectedPlayer)
                state.selectedPlayer = 0;
        });

    world.system<PlayerBroadcastBubble>("UpdateBroadcastBubbles")
        .kind(flecs::PostUpdate)
        .run([](flecs::iter &it) {
            while (it.next()) {
                auto bubbles = it.field<PlayerBroadcastBubble>(0);

                for (auto i : it) {
                    bubbles[i].remaining -= it.delta_time();
                    if (bubbles[i].remaining <= 0.0f) {
                        it.entity(i).remove<PlayerBroadcastBubble>();
                    }
                }
            }
        })
        .add<InScene>(sceneId<Game>(world));

    world.system<const Position3, const PlayerBroadcastBubble>("DrawBroadcastBubbles")
        .kind<RenderWorld2D>()
        .run([](flecs::iter &it) {
            while (it.next()) {
                auto positions = it.field<const Position3>(0);
                auto bubbles = it.field<const PlayerBroadcastBubble>(1);

                for (auto i : it) {
                    drawBroadcastBubble(it.world(), bubbles[i].message, Vector3{ positions[i].x, positions[i].y, positions[i].z });
                }
            }
        })
        .add<InScene>(sceneId<Game>(world));

    world.system<const Position3, const Player>("DrawPlayerTeamNames")
        .kind<RenderWorld2D>()
        .with<Player>()
        .run([world](flecs::iter &it) {
            const GuiSettings *settings = world.try_get<GuiSettings>();

            if (settings != nullptr && !settings->showTeamNames) {
                return;
            }

            while (it.next()) {
                auto positions = it.field<const Position3>(0);
                auto p_info = it.field<const Player>(1);

                for (auto i : it) {
                    flecs::entity player = it.entity(i);
                    flecs::entity team = player.target<BelongsTo>();

                    if (!team) {
                        continue;
                    }

                    const Team *teamData = team.try_get<Team>();
                    if (teamData == nullptr) {
                        continue;
                    }

                    drawPlayerLabel(world, std::format("{} | Lv {}", teamData->name, p_info[i].level), Vector3{ positions[i].x, positions[i].y, positions[i].z }, 20, SKYBLUE);
                }
            }
        })
        .add<InScene>(sceneId<Game>(world));

    world.system<const Team>("DrawTeamButtons")
        .kind<Render2D>()
        .run([world](flecs::iter &it) {
            auto &state = world.get_mut<GameUiState>();
            state.hoveredTeam = 0;
            Rectangle btn = { 200, 25, 200, 42 };

            while (it.next()) {
                auto teams = it.field<const Team>(0);
                for (auto i : it) {
                    if (CheckCollisionPointRec(GetMousePosition(), btn)) {
                        state.hoveredTeam = it.entity(i).id();
                    }
                    if (drawMinecraftButton(btn, teams[i].name.c_str(), 24)) {
                        state.openedTeam = (state.openedTeam == it.entity(i).id()) ? 0 : it.entity(i).id();
                        state.panelPositionX = btn.x;
                    }
                    btn.x += btn.width + 10;
                }
            }
        })
        .add<InScene>(sceneId<Game>(world));

    world.system<const Player, const zappy::Resources, const Texture2D>("DrawPlayerList")
        .kind<Render2D>()
        .order_by(0, [](flecs::entity_t e1, const void *, flecs::entity_t e2, const void *) {
            return (e1 > e2) - (e1 < e2);
        })
        .run([world](flecs::iter &it) {
            if (auto &state = world.get_mut<GameUiState>(); state.openedTeam != 0) {
                const Rectangle panel = { state.panelPositionX, 72, 230, 240 };

                DrawRectangleLinesEx(panel, 1.0f, Fade(WHITE, 0.35f));

                Rectangle btn = { state.panelPositionX + 8, panel.y + 8, panel.width - 16, 42 };

                while (it.next()) {
                    auto players = it.field<const Player>(0);
                    auto resources = it.field<const zappy::Resources>(1);
                    auto skins = it.field<const Texture2D>(2);

                    for (auto i : it) {
                        auto e = it.entity(i);
                        if (e.has<BelongsTo>(state.openedTeam)) {
                            if (GuiButton(btn, "")) {
                                state.selectedPlayer = e.id();
                                state.playerName = e.name().c_str();
                                state.level = players[i].level;
                                state.resources = resources[i];
                            }

                            const Rectangle head = { btn.x + 6, btn.y + 5, 32, 32 };
                            DrawMinecraftHead(skins[i], head);
                            DrawText(e.name().c_str(), static_cast<int>(btn.x + 46), static_cast<int>(btn.y + 7), 14, WHITE);
                            DrawText(std::format("Lv {}", players[i].level).c_str(), static_cast<int>(btn.x + 46), static_cast<int>(btn.y + 24), 12, Fade(WHITE, 0.75f));

                            btn.y += btn.height + 6;
                        }
                    }
                }
            }
        })
        .add<InScene>(sceneId<Game>(world));

    world.system("DrawInventoryModal")
        .kind<Render2D>()
        .run([world](flecs::iter &) {
            if (auto &state = world.get_mut<GameUiState>(); state.selectedPlayer != 0 && state.resources.has_value()) {
                const float inventoryScale = 2.0f;
                const Rectangle inventory = Position2::center(176.0f * inventoryScale, 166.0f * inventoryScale).rect(176.0f * inventoryScale, 166.0f * inventoryScale);
                const Rectangle closeButton = { inventory.x + inventory.width - 26.0f, inventory.y + 6.0f, 20.0f, 20.0f };

                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.55f));

                const GameAssets *assets = world.try_get<GameAssets>();
                if (assets != nullptr && assets->inventoryGuiTexture.id != 0) {
                    drawMinecraftInventory(*assets, state.resources.value(), inventory);
                    const float inventoryPixelScale = inventory.width / 176.0f;
                    const Rectangle skinPreview = {
                        inventory.x + 26.0f * inventoryPixelScale,
                        inventory.y + 8.0f * inventoryPixelScale,
                        48.0f * inventoryPixelScale,
                        70.0f * inventoryPixelScale,
                    };
                    if (world.is_alive(state.selectedPlayer)) {
                        const flecs::entity selectedPlayer(world, state.selectedPlayer);
                        if (const Texture2D *skin = selectedPlayer.try_get<Texture2D>(); skin != nullptr) {
                            DrawMinecraftPlayerPreview3D(*skin, skinPreview, GetMousePosition());
                        }
                    }

                    constexpr int headerFontSize = 20;
                    const std::string levelText = std::format("Level {}", state.level);
                    DrawText(state.playerName.c_str(), static_cast<int>(inventory.x + 16.0f), static_cast<int>(inventory.y - 28.0f), headerFontSize, WHITE);
                    DrawText(levelText.c_str(),
                             static_cast<int>(inventory.x + inventory.width - 16.0f - static_cast<float>(MeasureText(levelText.c_str(), headerFontSize))),
                             static_cast<int>(inventory.y - 28.0f),
                             headerFontSize,
                             WHITE);
                } else {
                    DrawRectangleRec(inventory, Fade(BLACK, 0.75f));
                    DrawRectangleLinesEx(inventory, 1.0f, WHITE);
                    DrawText("Inventory assets missing", static_cast<int>(inventory.x + 18.0f), static_cast<int>(inventory.y + 18.0f), 20, WHITE);
                }

                if (drawMinecraftButton(closeButton, "x", 14) || IsKeyPressed(KEY_ESCAPE)) {
                    state.selectedPlayer = 0;
                }
            }
        })
        .add<InScene>(sceneId<Game>(world));

    world.system("DrawFrequencySlider")
        .kind<Render2D>()
        .run([world](flecs::iter &) {
            auto &state = world.get_mut<GameUiState>();
            initializeFrequencyState(world, state);

            const float panelWidth = 360.0f;
            const Rectangle panel = { static_cast<float>(GetScreenWidth()) - panelWidth - 24.0f, 24.0f, panelWidth, 82.0f };
            const Rectangle slider = { panel.x + 44.0f, panel.y + 38.0f, panel.width - 112.0f, 24.0f };

            DrawRectangleRec(panel, Fade(BLACK, 0.42f));
            DrawRectangleLinesEx(panel, 1.0f, Fade(WHITE, 0.28f));
            DrawText("Freq", static_cast<int>(panel.x + 14.0f), static_cast<int>(panel.y + 14.0f), 20, WHITE);
            DrawText(std::format("{}", normalizedFrequency(state.requestedFrequency)).c_str(), static_cast<int>(panel.x + panel.width - 70.0f), static_cast<int>(panel.y + 14.0f), 20, SKYBLUE);

            GuiSlider(slider, "", "", &state.requestedFrequency,
                      static_cast<float>(minFrequency), static_cast<float>(maxFrequency));
            state.requestedFrequency = static_cast<float>(normalizedFrequency(state.requestedFrequency));

            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && state.requestedFrequency != state.confirmedFrequency) {
                const int requested = normalizedFrequency(state.requestedFrequency);

                if (requested != state.confirmedFrequency) {
                    sendServerCommand(world, std::format("sst {}\n", requested));
                }
            }
        })
        .add<InScene>(sceneId<Game>(world));
}
