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

/// Forward declaration for TCP line buffering.
class LineBuffer;
/// Forward declaration for the Zappy GUI handshake state.
class ZappySession;

/// Public connection state exposed to the ECS/UI layer.
enum class ClientStatus {
    /// No active network worker or socket.
    Disconnected,
    /// TCP connection is being opened.
    Connecting,
    /// Waiting for the Zappy GUI welcome exchange.
    Handshaking,
    /// GUI handshake completed and protocol lines are flowing.
    Connected,
    /// Last operation failed.
    Error
};

/// Threaded Zappy GUI client.
///
/// This class owns the TCP client on a worker thread. The main/Flecs thread
/// communicates with it by queueing outgoing commands and polling complete
/// incoming protocol lines.
class ZappyClient {
  public:
    /// Creates an idle disconnected client.
    ZappyClient() = default;
    /// Stops the worker thread before destruction.
    ~ZappyClient();

    /// Prevents copying the thread and queues.
    ZappyClient(const ZappyClient &) = delete;
    /// Prevents assigning the thread and queues.
    ZappyClient &operator=(const ZappyClient &) = delete;

    /// Starts a worker that connects to the given server.
    void start(std::string host, std::uint16_t port);
    /// Stops the worker and marks the client disconnected.
    void stop();

    /// Queues a command for the network thread.
    void send(std::string command);
    /// Pops one complete server line, if available.
    bool pollLine(std::string &out);
    /// Pops one network error message, if available.
    bool pollError(std::string &out);

    /// Returns the last known connection status.
    [[nodiscard]] ClientStatus getStatus() const;
    /// Returns the last known connection status message.
    [[nodiscard]] std::string getStatusMessage() const;

  private:
    /// Runs the worker loop until stop, disconnect, or error.
    void run(std::string host, std::uint16_t port, std::stop_token stopToken);
    /// Pushes one server line into the main-thread queue.
    void pushLine(std::string line);
    /// Pushes one error message into the main-thread queue.
    void pushError(std::string message);
    /// Pops one command queued by the main thread.
    bool popOutgoing(std::string &out);
    /// Updates status and message under the mutex.
    void setStatus(ClientStatus status, std::string message);
    /// Records an error and switches status to Error.
    void fail(std::string message);
    /// Creates a configured TCP client.
    Result<TcpClient> createClient(const std::string &host, uint16_t port);
    /// Reads and dispatches any available server data.
    bool receiveAvailable(TcpClient &client, LineBuffer &lines, ZappySession &session);
    /// Sends every queued outgoing command.
    bool flushOutgoing(TcpClient &client);

    /// Maximum number of queued lines, commands, or errors.
    static constexpr std::size_t maxQueueSize = 4096;
    /// Protects queues and visible status fields.
    mutable std::mutex mutex;
    /// Worker thread that owns the socket.
    std::jthread thread;
    /// Complete protocol lines received from the server.
    BoundedQueue<std::string, maxQueueSize> incoming;
    /// Commands waiting to be sent to the server.
    BoundedQueue<std::string, maxQueueSize> outgoing;
    /// Errors waiting to be displayed/handled by the ECS thread.
    BoundedQueue<std::string, maxQueueSize> errors;
    /// Last known client status.
    ClientStatus status = ClientStatus::Disconnected;
    /// Display-ready status details.
    std::string statusMessage = "Disconnected";
};

/// Shared pointer type stored in Flecs components.
using ZappyClientPtr = std::shared_ptr<ZappyClient>;

} // namespace net
