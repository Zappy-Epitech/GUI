/**
 * @file Scanner.hpp
 * @ingroup gui_parsing
 * @brief Cursor-based scanner for tokenising string views.
 */
#pragma once

#include <charconv>
#include <concepts>
#include <optional>
#include <string_view>

namespace parsing {

/**
 * @brief Reads tokens from a string view using a forward-only cursor.
 * @ingroup gui_parsing
 *
 * Maintains a position into an immutable input view and exposes primitives to
 * consume characters, literals and integers. Consuming operations advance the
 * cursor; on failure the cursor is left unchanged (or restored).
 *
 * @tparam Char Character type the scanner operates on (defaults to `char`).
 */
template <typename Char = char>
class Scanner {
  public:
    /// View type scanned by this scanner.
    using View = std::basic_string_view<Char>;

    /// Creates a scanner from a view.
    explicit Scanner(View input) : _input(input) {
    }

    /// Creates a scanner from a string.
    explicit Scanner(const Char *input) : _input(input) {
    }

    /// Checks whether all input was consumed.
    [[nodiscard]] bool isDone() const {
        return _cursor >= _input.size();
    }

    /// Returns the current cursor position.
    [[nodiscard]] size_t position() const {
        return _cursor;
    }

    /// Returns the current character.
    [[nodiscard]] std::optional<Char> peek() const {
        if (isDone()) {
            return std::nullopt;
        }
        return _input[_cursor];
    }

    /**
     * @brief Consumes one character if it matches the expected one.
     * @param expected Character that must be at the current cursor position.
     * @return `true` and advances the cursor by one when the current character
     *         equals @p expected; `false` with the cursor unchanged otherwise
     *         (including when the input is already exhausted).
     */
    bool take(Char expected) {
        if (peek() != expected) {
            return false;
        }
        _cursor++;
        return true;
    }

    /**
     * @brief Consumes an exact literal sequence starting at the cursor.
     * @param expected Literal that must appear at the current cursor position.
     * @return `true` and advances the cursor by `expected.size()` when the
     *         input at the cursor starts with @p expected; `false` with the
     *         cursor unchanged otherwise.
     */
    bool takeLiteral(View expected) {
        if (_input.substr(_cursor, expected.size()) != expected) {
            return false;
        }
        _cursor += expected.size();
        return true;
    }

    /**
     * @brief Consumes consecutive characters for which a predicate holds.
     * @tparam Predicate Callable invocable as `bool(Char)`.
     * @param predicate Tested against each character; consumption stops at the
     *        first character for which it returns `false` or at end of input.
     * @return View over the consumed characters (empty if none matched). The
     *         cursor is advanced past the returned characters.
     */
    template <typename Predicate>
    View takeWhile(Predicate predicate) {
        const size_t start = _cursor;
        while (!isDone() && predicate(_input[_cursor])) {
            _cursor++;
        }
        return _input.substr(start, _cursor - start);
    }

    /**
     * @brief Consumes characters until a predicate first holds.
     * @tparam Predicate Callable invocable as `bool(Char)`.
     * @param predicate Consumption stops at the first character for which it
     *        returns `true` (that character is left unconsumed) or at end of
     *        input.
     * @return View over the consumed characters (empty if the first character
     *         already satisfies @p predicate). The cursor is advanced past the
     *         returned characters.
     */
    template <typename Predicate>
    View takeUntil(Predicate predicate) {
        return takeWhile([&](Char value) {
            return !predicate(value);
        });
    }

    /// Consumes the remaining input.
    View takeRest() {
        const size_t start = _cursor;
        _cursor = _input.size();
        return _input.substr(start);
    }

    /// Skips spaces and tabs.
    void skipSpaces() {
        takeWhile([](Char value) {
            return value == ' ' || value == '\t';
        });
    }

    /// Consumes at least one space or tab.
    bool takeSpace() {
        const size_t start = _cursor;
        skipSpaces();
        return _cursor > start;
    }

    /**
     * @brief Reads an integer literal at the cursor.
     * @tparam Number Integral target type; for signed types an optional leading
     *         `-` sign is accepted.
     * @return The parsed value on success, with the cursor advanced past the
     *         digits (and sign). Returns `std::nullopt` and restores the cursor
     *         to its starting position when no digits are present or the value
     *         does not fully parse (e.g. out of range for @p Number).
     */
    template <std::integral Number>
    std::optional<Number> takeInteger() {
        const size_t start = _cursor;
        if constexpr (std::signed_integral<Number>) {
            take('-');
        }
        View digits = takeWhile([](Char value) {
            return value >= '0' && value <= '9';
        });
        if (digits.empty()) {
            _cursor = start;
            return std::nullopt;
        }

        Number value{};
        View token = _input.substr(start, _cursor - start);
        auto first = token.data();
        auto last = token.data() + token.size();
        auto result = std::from_chars(first, last, value);
        if (result.ec != std::errc{} || result.ptr != last) {
            _cursor = start;
            return std::nullopt;
        }
        return value;
    }

    /**
     * @brief Reads an integer that must be preceded by a given prefix character.
     * @tparam Number Integral target type passed through to takeInteger().
     * @param prefix Character that must immediately precede the integer.
     * @return The parsed value on success, with the cursor advanced past the
     *         prefix and the integer. Returns `std::nullopt` and restores the
     *         cursor to its starting position when the prefix is absent or no
     *         valid integer follows it.
     */
    template <std::integral Number>
    std::optional<Number> takePrefixedInteger(Char prefix) {
        const size_t start = _cursor;
        if (!take(prefix)) {
            return std::nullopt;
        }
        auto value = takeInteger<Number>();
        if (!value) {
            _cursor = start;
        }
        return value;
    }

  private:
    View _input;
    size_t _cursor = 0;
};

template <typename Char>
Scanner(const Char *) -> Scanner<Char>;

} // namespace parsing
