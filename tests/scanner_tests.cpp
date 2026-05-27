#include "src/parsing/Scanner.hpp"

#include <criterion/criterion.h>
#include <string_view>

Test(scanner, takes_literals_and_words) {
    parsing::Scanner scanner("pnw #12 TeamA");

    cr_assert(scanner.takeLiteral("pnw"));
    cr_assert(scanner.takeSpace());

    auto id = scanner.takePrefixedInteger<int>('#');
    cr_assert(id.has_value());
    cr_assert_eq(*id, 12);

    cr_assert(scanner.takeSpace());
    auto team = scanner.takeWhile([](char value) {
        return value != ' ';
    });

    cr_assert_eq(team, std::string_view("TeamA"));
    cr_assert(scanner.isDone());
}

Test(scanner, rolls_back_invalid_integer) {
    parsing::Scanner scanner("#abc");

    auto id = scanner.takePrefixedInteger<int>('#');

    cr_assert(!id.has_value());
    cr_assert_eq(scanner.position(), 0);
}

Test(scanner, takes_rest_after_message_prefix) {
    parsing::Scanner scanner("pbc #4 hello world");

    cr_assert(scanner.takeLiteral("pbc"));
    cr_assert(scanner.takeSpace());

    auto id = scanner.takePrefixedInteger<int>('#');
    cr_assert(id.has_value());
    cr_assert_eq(*id, 4);

    cr_assert(scanner.takeSpace());
    cr_assert_eq(scanner.takeRest(), std::string_view("hello world"));
    cr_assert(scanner.isDone());
}
