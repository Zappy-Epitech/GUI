#pragma once

#include "../utils/Result.hpp"

#include <string>
#include <string_view>

namespace net {

class TcpClient;

enum class ZappyLineAction {
    Consumed,
    Connected,
    Forward
};

/// Handles Zappy GUI transport setup before normal protocol lines are forwarded.
class ZappySession {
  public:
    static std::string formatCommand(std::string command);

    Result<ZappyLineAction> handleLine(TcpClient &client, std::string_view line);

  private:
    bool graphicSent = false;
};

} // namespace net
