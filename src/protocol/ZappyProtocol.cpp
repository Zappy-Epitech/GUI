/**
 * @file ZappyProtocol.cpp
 * @ingroup gui_protocol
 * @brief Implements line dispatch to per-command parsers for the Zappy protocol.
 */
#include "ZappyProtocol.hpp"
#include "CommandParsers.hpp"
#include "ParseUtils.hpp"

namespace zappy {

/// Parses one server protocol line.
std::optional<Event> ZappyProtocol::parseLine(std::string_view line) {
    ProtocolScanner scanner(trimLine(line));
    auto command = takeToken(scanner);
    if (!command) {
        return std::nullopt;
    }

    for (const auto &entry : commandParsers) {
        if (*command == entry.name) {
            return entry.parse(scanner);
        }
    }
    return std::nullopt;
}

/// Creates a default player event.
PlayerNew PlayerNew::withIdAndTeam(int id, const std::string &team) {
    return { .id = id, .x = 3, .y = 3, .orientation = Orientation::NORTH, .level = 1, .team = team };
}

} // namespace zappy
