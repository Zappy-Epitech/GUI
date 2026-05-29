#pragma once
#include "../parsing/Scanner.hpp"
#include "ZappyProtocol.hpp"

namespace zappy {

using ProtocolScanner = parsing::Scanner<>;

std::string_view trimLine(std::string_view line);
std::optional<std::string_view> takeToken(ProtocolScanner &scanner);
std::optional<int> takeInt(ProtocolScanner &scanner);
std::optional<int> takeId(ProtocolScanner &scanner);
std::optional<std::string> takeString(ProtocolScanner &scanner);
std::optional<std::string> takeMessage(ProtocolScanner &scanner);
std::optional<Resources> takeResources(ProtocolScanner &scanner);
bool isDone(ProtocolScanner &scanner);

} // namespace zappy
