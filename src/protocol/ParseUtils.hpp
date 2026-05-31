#pragma once
#include "../parsing/Scanner.hpp"
#include "ZappyProtocol.hpp"

namespace zappy {

/// Scanner type used by protocol parsers.
using ProtocolScanner = parsing::Scanner<>;

/// Removes line endings from a line.
std::string_view trimLine(std::string_view line);
/// Reads the next space-separated token.
std::optional<std::string_view> takeToken(ProtocolScanner &scanner);
/// Reads the next integer.
std::optional<int> takeInt(ProtocolScanner &scanner);
/// Reads the next prefixed player id.
std::optional<int> takeId(ProtocolScanner &scanner);
/// Reads the next token as a string.
std::optional<std::string> takeString(ProtocolScanner &scanner);
/// Reads the rest of the line as a message.
std::optional<std::string> takeMessage(ProtocolScanner &scanner);
/// Reads the seven resource counts.
std::optional<Resources> takeResources(ProtocolScanner &scanner);
/// Checks if the scanner reached the end.
bool isDone(ProtocolScanner &scanner);

} // namespace zappy
