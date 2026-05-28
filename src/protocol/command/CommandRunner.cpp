#include "CommandRunner.hpp"
#include "src/protocol/ZappyProtocol.hpp"
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
                invalid },
            event.value());
    } else {
        // TODO: handle error
    }
}
