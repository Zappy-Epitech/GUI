#include "ZappySession.hpp"
#include "../core/TcpClient.hpp"

#include <array>

namespace net {
namespace {

/// Initial GUI queries sent after GRAPHIC to populate the world.
constexpr std::array<std::string_view, 4> bootstrapCommands = {
    "msz\n",
    "mct\n",
    "tna\n",
    "sgt\n",
};

/// Sends GRAPHIC and the initial state requests required by the protocol.
Result<void> sendBootstrap(TcpClient &client) {
    if (auto sent = client.send("GRAPHIC\n"); !sent) {
        return sent;
    }

    for (std::string_view command : bootstrapCommands) {
        if (auto sent = client.send(command); !sent) {
            return sent;
        }
    }

    return {};
}

} // namespace

/// Ensures an outgoing command is newline-terminated.
std::string ZappySession::formatCommand(std::string command) {
    if (command.empty() || command.back() != '\n') {
        command.push_back('\n');
    }
    return command;
}

/// Handles the WELCOME handshake or forwards protocol lines.
Result<ZappyLineAction> ZappySession::handleLine(TcpClient &client, std::string_view line) {
    if (!graphicSent && line == "WELCOME") {
        if (auto sent = sendBootstrap(client); !sent) {
            return std::unexpected(sent.error());
        }
        graphicSent = true;
        return ZappyLineAction::Connected;
    }

    return ZappyLineAction::Forward;
}

} // namespace net
