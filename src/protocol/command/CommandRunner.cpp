#include "CommandRunner.hpp"
#include "src/protocol/ZappyProtocol.hpp"
#include "src/protocol/command/MapCommand.hpp"
#include "src/protocol/command/PlayerCommand.hpp"
#include <cstdio>
#include <optional>
#include <stdexcept>
#include <variant>

template <class... Ts>
struct Overloaded : Ts... {
    using Ts::operator()...;
};

template <class... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

auto invalid = [](const auto &) {
    throw std::invalid_argument("Unknown event type");
};

void runCommand(flecs::world &world, std::string &command) {
    std::optional<zappy::Event> event =
        zappy::ZappyProtocol::parseLine(command);

    if (event.has_value()) {
        std::visit(
            Overloaded{
                [&world](zappy::PlayerNew evt) {
                    applyPlayerNew(world, evt);
                },
                [&world](zappy::MapSize evt) {
                    applyMapNew(world, evt);
                },
                [&world](zappy::TileContent evt) {
                    applyTileContent(world, evt);
                },
                [&world](zappy::PlayerPosition evt) {
                    applyPlayerPosition(world, evt);
                },
                [&world](zappy::PlayerLevel evt) {
                    applyPlayerLevel(world, evt);
                },
                [&world](zappy::PlayerInventory evt) {
                    applyPlayerInventory(world, evt);
                },
                [&world](zappy::PlayerBroadcast evt) {
                    applyPlayerBroadcast(world, evt);
                },
                [&world](zappy::IncantationEnd evt) {
                    applyIncantationEnd(world, evt);
                },
                [&world](zappy::PlayerResourceDrop evt) {
                    applyPlayerResourceDrop(world, evt);
                },
                invalid },
            event.value());
    } else {
        // TODO: handle error
    }
}
