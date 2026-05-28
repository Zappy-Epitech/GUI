#include "PlayerCommand.hpp"
#include "src/modules/Player.hpp"
#include "src/modules/Spatial.hpp"
#include "src/protocol/ZappyProtocol.hpp"
#include <format>

void applyPlayerNew(flecs::world &world, zappy::PlayerNew &evt) {
    world.entity(std::format("Player({}, {})", evt.id, evt.team).c_str())
        .set(Player{ .level = evt.level })
        .set<Orientation>(evt.orientation)
        .set(Position3::from_xyz(evt.x * 100, evt.y * 100, 50));
}
