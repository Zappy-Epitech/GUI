#pragma once
#include "../io/BoundedQueue.hpp"
#include "../core/TcpClient.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

namespace net {

class LineBuffer;
class ZappySession;

/// Public connection state exposed to the ECS/UI layer.
enum class ClientStatus {
    Disconnected,
    Connecting,
    Handshaking,
    Connected,
    Error
};

/// Threaded Zappy GUI client.
///
/// This class owns the TCP client on a worker thread. The main/Flecs thread
/// communicates with it by queueing outgoing commands and polling complete
/// incoming protocol lines.
class ZappyClient {
  public:
    ZappyClient() = default;
    ~ZappyClient();

    ZappyClient(const ZappyClient &) = delete;
    ZappyClient &operator=(const ZappyClient &) = delete;

    void start(std::string host, std::uint16_t port);
    void stop();

    /// Queues a command for the network thread.
    void send(std::string command);
    /// Pops one complete server line, if available.
    bool pollLine(std::string &out);
    /// Pops one network error message, if available.
    bool pollError(std::string &out);

    [[nodiscard]] ClientStatus getStatus() const;
    [[nodiscard]] std::string getStatusMessage() const;

  private:
    void run(std::string host, std::uint16_t port, std::stop_token stopToken);
    void pushLine(std::string line);
    void pushError(std::string message);
    bool popOutgoing(std::string &out);
    void setStatus(ClientStatus status, std::string message);
    void fail(std::string message);
    Result<TcpClient> createClient(const std::string &host, uint16_t port);
    bool receiveAvailable(TcpClient &client, LineBuffer &lines, ZappySession &session);
    bool flushOutgoing(TcpClient &client);

    static constexpr std::size_t maxQueueSize = 4096;
    mutable std::mutex mutex;
    std::jthread thread;
    /// Complete protocol lines received from the server.
    BoundedQueue<std::string, maxQueueSize> incoming;
    /// Commands waiting to be sent to the server.
    BoundedQueue<std::string, maxQueueSize> outgoing;
    /// Errors waiting to be displayed/handled by the ECS thread.
    BoundedQueue<std::string, maxQueueSize> errors;
    ClientStatus status = ClientStatus::Disconnected;
    std::string statusMessage = "Disconnected";
};

using ZappyClientPtr = std::shared_ptr<ZappyClient>;

} // namespace net
