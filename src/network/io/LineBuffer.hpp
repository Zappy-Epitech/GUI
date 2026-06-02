#pragma once

#include <string>
#include <string_view>

namespace net {

/// Rebuilds newline-terminated protocol lines from TCP stream chunks.
class LineBuffer {
  public:
    void append(std::string_view chunk);
    bool popLine(std::string &out);
    void clear();

  private:
    std::string buffer;
};

} // namespace net
