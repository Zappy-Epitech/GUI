#include "CommandParsers.hpp"
#include "src/modules/Spatial.hpp"

namespace zappy {
namespace {

std::optional<Event> parseMapSize(ProtocolScanner &scanner) {
    auto width = takeInt(scanner);
    auto height = takeInt(scanner);
    if (!width || !height || !isDone(scanner)) {
        return std::nullopt;
    }
    return MapSize{ *width, *height };
}

std::optional<Event> parseTileContent(ProtocolScanner &scanner) {
    auto x = takeInt(scanner);
    auto y = takeInt(scanner);
    auto resources = takeResources(scanner);
    if (!x || !y || !resources || !isDone(scanner)) {
        return std::nullopt;
    }
    return TileContent{ *x, *y, *resources };
}

std::optional<Event> parseTeamName(ProtocolScanner &scanner) {
    auto name = takeString(scanner);
    if (!name || !isDone(scanner)) {
        return std::nullopt;
    }
    return TeamName{ *name };
}

std::optional<Event> parsePlayerNew(ProtocolScanner &scanner) {
    auto id = takeId(scanner);
    auto x = takeInt(scanner);
    auto y = takeInt(scanner);
    auto orientation = takeInt(scanner);
    auto level = takeInt(scanner);
    auto team = takeString(scanner);
    if (!id || !x || !y || !orientation || !level || !team || !isDone(scanner)) {
        return std::nullopt;
    }
    return PlayerNew{ *id, *x, *y, (Orientation) *orientation, *level, *team };
}

std::optional<Event> parsePlayerPosition(ProtocolScanner &scanner) {
    auto id = takeId(scanner);
    auto x = takeInt(scanner);
    auto y = takeInt(scanner);
    auto orientation = takeInt(scanner);
    if (!id || !x || !y || !orientation || !isDone(scanner)) {
        return std::nullopt;
    }
    return PlayerPosition{ *id, *x, *y, (Orientation) *orientation };
}

std::optional<Event> parsePlayerLevel(ProtocolScanner &scanner) {
    auto id = takeId(scanner);
    auto level = takeInt(scanner);
    if (!id || !level || !isDone(scanner)) {
        return std::nullopt;
    }
    return PlayerLevel{ *id, *level };
}

std::optional<Event> parsePlayerInventory(ProtocolScanner &scanner) {
    auto id = takeId(scanner);
    auto x = takeInt(scanner);
    auto y = takeInt(scanner);
    auto resources = takeResources(scanner);
    if (!id || !x || !y || !resources || !isDone(scanner)) {
        return std::nullopt;
    }
    return PlayerInventory{ *id, *x, *y, *resources };
}

template <typename EventType>
std::optional<Event> parsePlayerId(ProtocolScanner &scanner) {
    auto id = takeId(scanner);
    if (!id || !isDone(scanner)) {
        return std::nullopt;
    }
    return EventType{ *id };
}

template <typename EventType>
std::optional<Event> parsePlayerResource(ProtocolScanner &scanner) {
    auto id = takeId(scanner);
    auto resource = takeInt(scanner);
    if (!id || !resource || !isDone(scanner)) {
        return std::nullopt;
    }
    return EventType{ *id, *resource };
}

std::optional<Event> parsePlayerBroadcast(ProtocolScanner &scanner) {
    auto id = takeId(scanner);
    auto message = takeMessage(scanner);
    if (!id || !message) {
        return std::nullopt;
    }
    return PlayerBroadcast{ *id, *message };
}

std::optional<Event> parseIncantationStart(ProtocolScanner &scanner) {
    auto x = takeInt(scanner);
    auto y = takeInt(scanner);
    auto level = takeInt(scanner);
    if (!x || !y || !level) {
        return std::nullopt;
    }

    std::vector<int> ids;
    while (!isDone(scanner)) {
        auto id = takeId(scanner);
        if (!id) {
            return std::nullopt;
        }
        ids.push_back(*id);
    }
    if (ids.empty()) {
        return std::nullopt;
    }
    return IncantationStart{ *x, *y, *level, ids };
}

std::optional<Event> parseIncantationEnd(ProtocolScanner &scanner) {
    auto x = takeInt(scanner);
    auto y = takeInt(scanner);
    auto result = takeInt(scanner);
    if (!x || !y || !result || !isDone(scanner)) {
        return std::nullopt;
    }
    return IncantationEnd{ *x, *y, *result != 0 };
}

std::optional<Event> parseEggNew(ProtocolScanner &scanner) {
    auto id = takeId(scanner);
    auto playerId = takeId(scanner);
    auto x = takeInt(scanner);
    auto y = takeInt(scanner);
    if (!id || !playerId || !x || !y || !isDone(scanner)) {
        return std::nullopt;
    }
    return EggNew{ *id, *playerId, *x, *y };
}

template <typename EventType>
std::optional<Event> parseEggId(ProtocolScanner &scanner) {
    auto id = takeId(scanner);
    if (!id || !isDone(scanner)) {
        return std::nullopt;
    }
    return EventType{ *id };
}

std::optional<Event> parseTimeUnit(ProtocolScanner &scanner) {
    auto value = takeInt(scanner);
    if (!value || !isDone(scanner)) {
        return std::nullopt;
    }
    return TimeUnit{ *value };
}

std::optional<Event> parseGameEnd(ProtocolScanner &scanner) {
    auto winner = takeString(scanner);
    if (!winner || !isDone(scanner)) {
        return std::nullopt;
    }
    return GameEnd{ *winner };
}

std::optional<Event> parseServerMessage(ProtocolScanner &scanner) {
    auto message = takeMessage(scanner);
    if (!message) {
        return std::nullopt;
    }
    return ServerMessage{ *message };
}

template <typename EventType>
std::optional<Event> parseEmpty(ProtocolScanner &scanner) {
    if (!isDone(scanner)) {
        return std::nullopt;
    }
    return EventType{};
}

} // namespace

const std::array<CommandParser, 24> commandParsers = {
    CommandParser{ "msz", parseMapSize },
    CommandParser{ "bct", parseTileContent },
    CommandParser{ "tna", parseTeamName },
    CommandParser{ "pnw", parsePlayerNew },
    CommandParser{ "ppo", parsePlayerPosition },
    CommandParser{ "plv", parsePlayerLevel },
    CommandParser{ "pin", parsePlayerInventory },
    CommandParser{ "pex", parsePlayerId<PlayerExpelled> },
    CommandParser{ "pbc", parsePlayerBroadcast },
    CommandParser{ "pic", parseIncantationStart },
    CommandParser{ "pie", parseIncantationEnd },
    CommandParser{ "pfk", parsePlayerId<PlayerEggLayStart> },
    CommandParser{ "pdr", parsePlayerResource<PlayerResourceDrop> },
    CommandParser{ "pgt", parsePlayerResource<PlayerResourceCollect> },
    CommandParser{ "pdi", parsePlayerId<PlayerDeath> },
    CommandParser{ "enw", parseEggNew },
    CommandParser{ "ebo", parseEggId<EggHatched> },
    CommandParser{ "edi", parseEggId<EggDeath> },
    CommandParser{ "sgt", parseTimeUnit },
    CommandParser{ "sst", parseTimeUnit },
    CommandParser{ "seg", parseGameEnd },
    CommandParser{ "smg", parseServerMessage },
    CommandParser{ "suc", parseEmpty<UnknownCommand> },
    CommandParser{ "sbp", parseEmpty<BadParameter> },
};

} // namespace zappy
