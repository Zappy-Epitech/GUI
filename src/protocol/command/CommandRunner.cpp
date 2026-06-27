#include "CommandRunner.hpp"
#include "src/core/IncantationEffect.hpp"
#include "src/protocol/ZappyProtocol.hpp"
#include "src/protocol/command/EggCommand.hpp"
#include "src/protocol/command/MapCommand.hpp"
#include "src/protocol/command/PlayerCommand.hpp"
#include "src/protocol/command/ServerCommand.hpp"
#include "src/protocol/command/TeamCommand.hpp"
#include <cstdlib>
#include <iostream>
#include <optional>
#include <variant>

/// Combines variant visitors.
template <class... Ts>
struct Overloaded : Ts... {
    using Ts::operator()...;
};

template <class... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

/// Reports unsupported events.
auto invalid = [](const auto &) {
    exit(1);
};

/// Parses and applies one command.
void runCommand(const flecs::world &world, const std::string &command) {
    // std::cout << "runCommand: " << command << std::endl;
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
                [&world](zappy::TeamName evt) {
                    applyTeamName(world, evt);
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
                [&world](zappy::PlayerExpelled evt) {
                    applyPlayerExpelled(world, evt);
                },
                [&world](zappy::PlayerBroadcast evt) {
                    applyPlayerBroadcast(world, evt);
                },
                [&world](zappy::IncantationStart evt) {
                    applyIncantationStart(world, evt);
                    startIncantationEffect(world, evt.x, evt.y, evt.level, evt.playerIds);
                },
                [&world](zappy::IncantationEnd evt) {
                    applyIncantationEnd(world, evt);
                    finishIncantationEffect(world, evt.x, evt.y, evt.success);
                },
                [&world](zappy::PlayerEggLayStart evt) {
                    applyPlayerEggLayStart(world, evt);
                },
                [&world](zappy::PlayerResourceDrop evt) {
                    applyPlayerResourceDrop(world, evt);
                },
                [&world](zappy::PlayerResourceCollect evt) {
                    applyPlayerResourceCollect(world, evt);
                },
                [&world](zappy::PlayerDeath evt) {
                    applyPlayerDeath(world, evt);
                },
                [&world](zappy::EggNew evt) {
                    applyEggNew(world, evt);
                },
                [&world](zappy::EggHatched evt) {
                    applyEggHatched(world, evt);
                },
                [&world](zappy::EggDeath evt) {
                    applyEggDeath(world, evt);
                },
                [&world](zappy::TimeUnit evt) {
                    applyTimeUnit(world, evt);
                },
                [&world](zappy::GameEnd evt) {
                    applyGameEnd(world, evt);
                },
                [&world](zappy::ServerMessage evt) {
                    applyServerMessage(world, evt);
                },
                [&world](zappy::UnknownCommand evt) {
                    applyUnknownCommand(world, evt);
                },
                [&world](zappy::BadParameter evt) {
                    applyBadParameter(world, evt);
                },
                invalid },
            event.value());
    } else {
        // TODO: handle error
    }
}
