#pragma once

#include "ParseUtils.hpp"

#include <array>

namespace zappy {

using CommandParserFn = std::optional<Event> (*)(ProtocolScanner &);

struct CommandParser {
    std::string_view name;
    CommandParserFn parse;
};

extern const std::array<CommandParser, 24> commandParsers;

} // namespace zappy
