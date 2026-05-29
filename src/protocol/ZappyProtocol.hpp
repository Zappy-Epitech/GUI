#pragma once
#include "src/modules/Spatial.hpp"
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace zappy {

struct Resources {
    int food;
    int linemate;
    int deraumere;
    int sibur;
    int mendiane;
    int phiras;
    int thystame;
};

struct MapSize {
    int width;
    int height;
};

struct TileContent {
    int x;
    int y;
    Resources resources;
};

struct TeamName {
    std::string name;
};

struct PlayerNew {
    int id;
    int x;
    int y;
    Orientation orientation;
    int level;
    std::string team;
};

struct PlayerPosition {
    int id;
    int x;
    int y;
    Orientation orientation;
};

struct PlayerLevel {
    int id;
    int level;
};

struct PlayerInventory {
    int id;
    int x;
    int y;
    Resources resources;
};

struct PlayerExpelled {
    int id;
};

struct PlayerBroadcast {
    int id;
    std::string message;
};

struct IncantationStart {
    int x;
    int y;
    int level;
    std::vector<int> playerIds;
};

struct IncantationEnd {
    int x;
    int y;
    bool success;
};

struct PlayerEggLayStart {
    int id;
};

struct PlayerResourceDrop {
    int id;
    int resource;
};

struct PlayerResourceCollect {
    int id;
    int resource;
};

struct PlayerDeath {
    int id;
};

struct EggNew {
    int id;
    int playerId;
    int x;
    int y;
};

struct EggHatched {
    int id;
};

struct EggDeath {
    int id;
};

struct TimeUnit {
    int value;
};

struct GameEnd {
    std::string winner;
};

struct ServerMessage {
    std::string message;
};

struct UnknownCommand {};

struct BadParameter {};

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

class ZappyProtocol {
  public:
    static std::optional<Event> parseLine(std::string_view line);
};

} // namespace zappy
