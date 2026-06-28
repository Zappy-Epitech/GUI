/**
 * @file ZappySession.hpp
 * @ingroup gui_network
 * @brief Handles the Zappy GUI WELCOME/GRAPHIC handshake before forwarding lines.
 */
#pragma once

#include "../utils/Result.hpp"

#include <string>
#include <string_view>

namespace net {

/// Forward declaration for the socket wrapper used by the session.
class TcpClient;

/// Describes how a received line should be handled.
enum class ZappyLineAction {
    /// Line was handled internally.
    Consumed,
    /// GUI handshake has completed.
    Connected,
    /// Line should be forwarded to the protocol parser.
    Forward
};

/** @brief Handles Zappy GUI transport setup before normal protocol lines are forwarded.
 * @ingroup gui_network
 */
class ZappySession {
  public:
    /// Ensures an outgoing command is newline-terminated.
    static std::string formatCommand(std::string command);

    /// Handles WELCOME/GRAPHIC setup or forwards normal protocol lines.
    Result<ZappyLineAction> handleLine(TcpClient &client, std::string_view line);

  private:
    /// Tracks whether GRAPHIC and bootstrap commands were sent.
    bool graphicSent = false;
};

} // namespace net
