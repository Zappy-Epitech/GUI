/**
 * @file ParseUtils.hpp
 * @ingroup gui_protocol
 * @brief Low-level token helpers for parsing Zappy server protocol lines.
 */
#pragma once
#include "../parsing/Scanner.hpp"
#include "ZappyProtocol.hpp"

namespace zappy {

/// Scanner type used by protocol parsers.
using ProtocolScanner = parsing::Scanner<>;

/**
 * @brief Removes trailing line endings (`\n` and `\r`) from a line.
 * @param line The raw line, possibly terminated by carriage returns/newlines.
 * @return A view of @p line with all trailing `\n`/`\r` characters stripped.
 */
std::string_view trimLine(std::string_view line);
/**
 * @brief Reads the next space-separated token.
 *
 * Skips leading spaces, then consumes characters up to the next space or tab.
 * @param scanner Scanner to read from; advanced past the token on success.
 * @return The token view, or `std::nullopt` if no non-empty token remains.
 */
std::optional<std::string_view> takeToken(ProtocolScanner &scanner);
/**
 * @brief Reads the next integer.
 *
 * Skips leading spaces, then parses a base-10 integer.
 * @param scanner Scanner to read from; advanced past the integer on success.
 * @return The parsed integer, or `std::nullopt` if no integer is present.
 */
std::optional<int> takeInt(ProtocolScanner &scanner);
/**
 * @brief Reads the next `#`-prefixed player/egg id.
 *
 * Skips leading spaces, then expects a `#` followed by an integer.
 * @param scanner Scanner to read from; advanced past the id on success.
 * @return The numeric id (without the `#`), or `std::nullopt` if absent/malformed.
 */
std::optional<int> takeId(ProtocolScanner &scanner);
/**
 * @brief Reads the next token and copies it into a string.
 * @param scanner Scanner to read from; advanced past the token on success.
 * @return The token as an owned string, or `std::nullopt` if no token remains.
 */
std::optional<std::string> takeString(ProtocolScanner &scanner);
/**
 * @brief Reads the remainder of the line as a free-form message.
 *
 * Requires a single leading space separator, then consumes the rest of the line.
 * @param scanner Scanner to read from; left at end of input on success.
 * @return The remaining text, or `std::nullopt` if the leading space is missing.
 */
std::optional<std::string> takeMessage(ProtocolScanner &scanner);
/**
 * @brief Reads the seven Zappy resource counts in protocol order.
 *
 * Parses food, linemate, deraumere, sibur, mendiane, phiras and thystame as
 * integers via takeInt().
 * @param scanner Scanner to read from; advanced past all seven values on success.
 * @return A filled Resources, or `std::nullopt` if any of the seven is missing.
 */
std::optional<Resources> takeResources(ProtocolScanner &scanner);
/**
 * @brief Checks whether the scanner has no further arguments.
 *
 * Skips trailing spaces, then tests for end of input. Used to reject lines with
 * unexpected extra tokens.
 * @param scanner Scanner to inspect; advanced past any trailing spaces.
 * @return `true` if nothing but spaces remains, `false` otherwise.
 */
bool isDone(ProtocolScanner &scanner);

} // namespace zappy
