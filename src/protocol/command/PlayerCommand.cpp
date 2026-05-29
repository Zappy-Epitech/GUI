#include "PlayerCommand.hpp"
#include "../../extern/flecs.hpp"
#include "src/modules/SkinAnimation.hpp"
#include "src/modules/Spatial.hpp"
#include "src/modules/gameplay/GameAssets.hpp"
#include "src/modules/gameplay/Player.hpp"
#include "src/modules/rendering/MinecraftSkinRenderer.hpp"
#include "src/protocol/ZappyProtocol.hpp"
#include <format>
#include <raylib.h>

static constexpr Size2 gridCellSize = { 1, 1 };
static constexpr float gridCellPadding = 0.1f;

static Position3 gridCenterPosition(int x, int y) {
    return Position3::from_xyz(
        static_cast<float>(x) * (gridCellSize.width + gridCellPadding),
        1.f,
        static_cast<float>(y) * (gridCellSize.height + gridCellPadding));
}

void applyPlayerNew(flecs::world &world, zappy::PlayerNew &evt) {
    const GameAssets &skins = world.get<GameAssets>();
    Texture2D skin = skins.skins[evt.id % skins.skins.size()];

    world.entity(std::format("Player({}, {})", evt.id, evt.team).c_str())
        .set(Player{ .level = evt.level })
        .set(skin)
        .set<Orientation>(evt.orientation)
        .set(gridCenterPosition(evt.x, evt.y))
        .set<Texture2D>(world.get<GameAssets>().skins[0])
        .set<MinecraftSkin>({ .scale = 0.3f })
        .set(Walking);
}
