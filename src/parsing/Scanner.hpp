#pragma once

#include <charconv>
#include <concepts>
#include <optional>
#include <string_view>

namespace parsing {

/// Reads tokens from a string view.
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

    /// Consumes one expected character.
    bool take(Char expected) {
        if (peek() != expected) {
            return false;
        }
        _cursor++;
        return true;
    }

    /// Consumes an expected literal.
    bool takeLiteral(View expected) {
        if (_input.substr(_cursor, expected.size()) != expected) {
            return false;
        }
        _cursor += expected.size();
        return true;
    }

    /// Consumes characters while a predicate matches.
    template <typename Predicate>
    View takeWhile(Predicate predicate) {
        const size_t start = _cursor;
        while (!isDone() && predicate(_input[_cursor])) {
            _cursor++;
        }
        return _input.substr(start, _cursor - start);
    }

    /// Consumes characters until a predicate matches.
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

    /// Reads an integer value.
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

    /// Reads an integer with a prefix.
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
