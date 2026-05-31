#pragma once
#include "src/core/Spatial.hpp"
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace zappy {

/// Stores resource counts.
struct Resources {
    int food = 0;
    int linemate = 0;
    int deraumere = 0;
    int sibur = 0;
    int mendiane = 0;
    int phiras = 0;
    int thystame = 0;
};

/// Stores map dimensions.
struct MapSize {
    int width;
    int height;
};

/// Stores tile resources.
struct TileContent {
    int x;
    int y;
    Resources resources;
};

/// Stores a team name event.
struct TeamName {
    std::string name;
};

/// Stores a new player event.
struct PlayerNew {
    int id;
    int x;
    int y;
    Orientation orientation;
    int level;
    std::string team;

    /// Creates a default player for a team.
    static PlayerNew withIdAndTeam(int id, const std::string &team);
};

/// Stores a player position event.
struct PlayerPosition {
    int id;
    int x;
    int y;
    Orientation orientation;
};

/// Stores a player level event.
struct PlayerLevel {
    int id;
    int level;
};

/// Stores a player inventory event.
struct PlayerInventory {
    int id;
    int x;
    int y;
    Resources resources;
};

/// Stores a player expulsion event.
struct PlayerExpelled {
    int id;
};

/// Stores a player broadcast event.
struct PlayerBroadcast {
    int id;
    std::string message;
};

/// Stores an incantation start event.
struct IncantationStart {
    int x;
    int y;
    int level;
    std::vector<int> playerIds;
};

/// Stores an incantation end event.
struct IncantationEnd {
    int x;
    int y;
    bool success;
};

/// Stores an egg laying start event.
struct PlayerEggLayStart {
    int id;
};

/// Stores a resource drop event.
struct PlayerResourceDrop {
    int id;
    int resource;
};

/// Stores a resource collect event.
struct PlayerResourceCollect {
    int id;
    int resource;
};

/// Stores a player death event.
struct PlayerDeath {
    int id;
};

/// Stores a new egg event.
struct EggNew {
    int id;
    int playerId;
    int x;
    int y;
};

/// Stores an egg hatch event.
struct EggHatched {
    int id;
};

/// Stores an egg death event.
struct EggDeath {
    int id;
};

/// Stores a time unit event.
struct TimeUnit {
    int value;
};

/// Stores a game end event.
struct GameEnd {
    std::string winner;
};

/// Stores a server message event.
struct ServerMessage {
    std::string message;
};

/// Marks an unknown command event.
struct UnknownCommand {};

/// Marks a bad parameter event.
struct BadParameter {};

/// Groups all protocol events.
using Event = std::variant<
    MapSize,
    TileContent,
    TeamName,
    PlayerNew,
    PlayerPosition,
    PlayerLevel,
    PlayerInventory,
    PlayerExpelled,
    PlayerBroadcast,
    IncantationStart,
    IncantationEnd,
    PlayerEggLayStart,
    PlayerResourceDrop,
    PlayerResourceCollect,
    PlayerDeath,
    EggNew,
    EggHatched,
    EggDeath,
    TimeUnit,
    GameEnd,
    ServerMessage,
    UnknownCommand,
    BadParameter>;

/// Parses Zappy server protocol lines.
class ZappyProtocol {
  public:
    /// Parses one protocol line.
    static std::optional<Event> parseLine(std::string_view line);
};

} // namespace zappy
