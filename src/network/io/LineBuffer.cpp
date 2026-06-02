#include "LineBuffer.hpp"

namespace net {

void LineBuffer::append(std::string_view chunk) {
    buffer += chunk;
}

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

void LineBuffer::clear() {
    buffer.clear();
}

} // namespace net
