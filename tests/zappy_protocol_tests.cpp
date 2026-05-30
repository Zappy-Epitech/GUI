#include "src/core/Spatial.hpp"
#include "src/protocol/ZappyProtocol.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/test.h>
#include <string>
#include <variant>

template <typename T>
static const T *parseAs(std::string_view line) {
    static std::optional<zappy::Event> event;

    event = zappy::ZappyProtocol::parseLine(line);
    cr_assert(event.has_value());
    return std::get_if<T>(&*event);
}

Test(zappy_protocol, parses_map_size) {
    const auto *event = parseAs<zappy::MapSize>("msz 10 20\n");

    cr_assert_not_null(event);
    cr_assert_eq(event->width, 10);
    cr_assert_eq(event->height, 20);
}

Test(zappy_protocol, parses_tile_content) {
    const auto *event = parseAs<zappy::TileContent>("bct 2 3 9 1 2 3 4 5 6");

    cr_assert_not_null(event);
    cr_assert_eq(event->x, 2);
    cr_assert_eq(event->y, 3);
    cr_assert_eq(event->resources.food, 9);
    cr_assert_eq(event->resources.linemate, 1);
    cr_assert_eq(event->resources.deraumere, 2);
    cr_assert_eq(event->resources.sibur, 3);
    cr_assert_eq(event->resources.mendiane, 4);
    cr_assert_eq(event->resources.phiras, 5);
    cr_assert_eq(event->resources.thystame, 6);
}

Test(zappy_protocol, parses_new_player) {
    const auto *event = parseAs<zappy::PlayerNew>("pnw #42 6 7 2 3 TeamA");

    cr_assert_not_null(event);
    cr_assert_eq(event->id, 42);
    cr_assert_eq(event->x, 6);
    cr_assert_eq(event->y, 7);
    cr_assert_eq(event->orientation, Orientation::EAST);
    cr_assert_eq(event->level, 3);
    cr_assert_eq(event->team, std::string("TeamA"));
}

Test(zappy_protocol, parses_player_inventory) {
    const auto *event = parseAs<zappy::PlayerInventory>("pin #5 1 2 3 4 5 6 7 8 9");

    cr_assert_not_null(event);
    cr_assert_eq(event->id, 5);
    cr_assert_eq(event->x, 1);
    cr_assert_eq(event->y, 2);
    cr_assert_eq(event->resources.food, 3);
    cr_assert_eq(event->resources.thystame, 9);
}

Test(zappy_protocol, parses_broadcast_with_spaces) {
    const auto *event = parseAs<zappy::PlayerBroadcast>("pbc #8 regroup at tile 3 4");

    cr_assert_not_null(event);
    cr_assert_eq(event->id, 8);
    cr_assert_eq(event->message, std::string("regroup at tile 3 4"));
}

Test(zappy_protocol, parses_incantation_start_with_players) {
    const auto *event = parseAs<zappy::IncantationStart>("pic 4 5 2 #1 #2 #3");

    cr_assert_not_null(event);
    cr_assert_eq(event->x, 4);
    cr_assert_eq(event->y, 5);
    cr_assert_eq(event->level, 2);
    cr_assert_eq(event->playerIds.size(), 3);
    cr_assert_eq(event->playerIds[0], 1);
    cr_assert_eq(event->playerIds[1], 2);
    cr_assert_eq(event->playerIds[2], 3);
}

Test(zappy_protocol, parses_egg_and_end_events) {
    const auto *egg = parseAs<zappy::EggNew>("enw #7 #2 8 9");
    cr_assert_not_null(egg);
    cr_assert_eq(egg->id, 7);
    cr_assert_eq(egg->playerId, 2);
    cr_assert_eq(egg->x, 8);
    cr_assert_eq(egg->y, 9);

    const auto *end = parseAs<zappy::GameEnd>("seg TeamB");
    cr_assert_not_null(end);
    cr_assert_eq(end->winner, std::string("TeamB"));
}

Test(zappy_protocol, parses_pnw_event) {
    const auto *event = parseAs<zappy::PlayerNew>("pnw #12 1 1 1 1 sasa");
    cr_assert_not_null(event);
    cr_assert_eq(event->id, 12);
}

Test(zappy_protocol, rejects_malformed_lines) {
    cr_assert(!zappy::ZappyProtocol::parseLine("").has_value());
    cr_assert(!zappy::ZappyProtocol::parseLine("msz 10").has_value());
    cr_assert(!zappy::ZappyProtocol::parseLine("pnw 1 2 3 4 5 TeamA").has_value());
    cr_assert(!zappy::ZappyProtocol::parseLine("bct 0 0 1 2 3").has_value());
}
