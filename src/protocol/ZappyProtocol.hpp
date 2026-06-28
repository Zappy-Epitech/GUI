/**
 * @file ZappyProtocol.hpp
 * @ingroup gui_protocol
 * @brief Protocol event types and the line parser for the Zappy server feed.
 */
#pragma once
#include "src/core/Spatial.hpp"
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace zappy {

/**
 * @brief Stores the seven Zappy resource counts.
 * @ingroup gui_protocol
 */
struct Resources {
    int food = 0;      ///< Number of food units.
    int linemate = 0;  ///< Number of linemate stones.
    int deraumere = 0; ///< Number of deraumere stones.
    int sibur = 0;     ///< Number of sibur stones.
    int mendiane = 0;  ///< Number of mendiane stones.
    int phiras = 0;    ///< Number of phiras stones.
    int thystame = 0;  ///< Number of thystame stones.
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

/**
 * @brief Parses Zappy server protocol lines into typed events.
 * @ingroup gui_protocol
 */
class ZappyProtocol {
  public:
    /**
     * @brief Parses one server protocol line into an Event.
     *
     * Trims line endings, reads the leading command token and dispatches to the
     * matching command parser, which validates the arguments.
     * @param line A single protocol line (with or without trailing `\n`/`\r`).
     * @return The parsed zappy::Event variant on success; `std::nullopt` if the
     *         line is empty, the command is unknown, or its arguments are invalid.
     */
    static std::optional<Event> parseLine(std::string_view line);
};

} // namespace zappy
