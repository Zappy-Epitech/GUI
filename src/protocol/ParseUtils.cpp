#include "ParseUtils.hpp"

namespace zappy {

/// Removes trailing line endings.
std::string_view trimLine(std::string_view line) {
    while (!line.empty() && (line.back() == '\n' || line.back() == '\r')) {
        line.remove_suffix(1);
    }
    return line;
}

/// Reads the next token.
std::optional<std::string_view> takeToken(ProtocolScanner &scanner) {
    scanner.skipSpaces();
    auto token = scanner.takeWhile([](char value) {
        return value != ' ' && value != '\t';
    });
    if (token.empty()) {
        return std::nullopt;
    }
    return token;
}

/// Reads the next integer.
std::optional<int> takeInt(ProtocolScanner &scanner) {
    scanner.skipSpaces();
    return scanner.takeInteger<int>();
}

/// Reads the next player id.
std::optional<int> takeId(ProtocolScanner &scanner) {
    scanner.skipSpaces();
    return scanner.takePrefixedInteger<int>('#');
}

/// Reads the next token as a string.
std::optional<std::string> takeString(ProtocolScanner &scanner) {
    auto token = takeToken(scanner);
    if (!token) {
        return std::nullopt;
    }
    return std::string(*token);
}

/// Reads the remaining message text.
std::optional<std::string> takeMessage(ProtocolScanner &scanner) {
    if (!scanner.takeSpace()) {
        return std::nullopt;
    }
    return std::string(scanner.takeRest());
}

/// Checks for trailing arguments.
bool isDone(ProtocolScanner &scanner) {
    scanner.skipSpaces();
    return scanner.isDone();
}

/// Reads all resource values.
std::optional<Resources> takeResources(ProtocolScanner &scanner) {
    auto food = takeInt(scanner);
    auto linemate = takeInt(scanner);
    auto deraumere = takeInt(scanner);
    auto sibur = takeInt(scanner);
    auto mendiane = takeInt(scanner);
    auto phiras = takeInt(scanner);
    auto thystame = takeInt(scanner);

    if (!food || !linemate || !deraumere || !sibur || !mendiane || !phiras || !thystame) {
        return std::nullopt;
    }
    return Resources{ *food, *linemate, *deraumere, *sibur, *mendiane, *phiras, *thystame };
}

} // namespace zappy
