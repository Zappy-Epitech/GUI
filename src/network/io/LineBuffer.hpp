#pragma once

#include <string>
#include <string_view>

namespace net {

/// Rebuilds newline-terminated protocol lines from TCP stream chunks.
class LineBuffer {
  public:
    /// Appends a raw TCP chunk.
    void append(std::string_view chunk);
    /// Extracts one newline-terminated line if available.
    bool popLine(std::string &out);
    /// Drops buffered partial data.
    void clear();

  private:
    /// Partial TCP stream data waiting for a newline.
    std::string buffer;
};

} // namespace net
