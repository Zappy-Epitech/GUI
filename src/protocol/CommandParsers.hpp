/**
 * @file CommandParsers.hpp
 * @ingroup gui_protocol
 * @brief Table mapping Zappy command names to their argument parsers.
 */
#pragma once

#include "ParseUtils.hpp"

#include <array>

namespace zappy {

/// Parses command arguments into an event.
using CommandParserFn = std::optional<Event> (*)(ProtocolScanner &);

/**
 * @brief Maps a command name to its argument parser.
 * @ingroup gui_protocol
 */
struct CommandParser {
    std::string_view name;
    CommandParserFn parse;
};

/// Lists all supported command parsers.
extern const std::array<CommandParser, 24> commandParsers;

} // namespace zappy
