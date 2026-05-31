#pragma once

#include "ParseUtils.hpp"

#include <array>

namespace zappy {

/// Parses command arguments into an event.
using CommandParserFn = std::optional<Event> (*)(ProtocolScanner &);

/// Maps a command name to its parser.
struct CommandParser {
    std::string_view name;
    CommandParserFn parse;
};

/// Lists all supported command parsers.
extern const std::array<CommandParser, 24> commandParsers;

} // namespace zappy
