#include "LineBuffer.hpp"

namespace net {

/// Appends a raw TCP chunk.
void LineBuffer::append(std::string_view chunk) {
    buffer += chunk;
}

/// Extracts one newline-terminated line if available.
bool LineBuffer::popLine(std::string &out) {
    const std::size_t pos = buffer.find('\n');
    if (pos == std::string::npos) {
        return false;
    }

    out = buffer.substr(0, pos);
    buffer.erase(0, pos + 1);

    if (!out.empty() && out.back() == '\r') {
        out.pop_back();
    }

    return true;
}

/// Drops buffered partial data.
void LineBuffer::clear() {
    buffer.clear();
}

} // namespace net
