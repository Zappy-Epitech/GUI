/**
 * @file ZappyClient.hpp
 * @ingroup gui_network
 * @brief Threaded Zappy GUI client exchanging protocol lines via queues.
 */
#pragma once
#include "../io/BoundedQueue.hpp"
#include "../io/UnboundedQueue.hpp"
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

/** @brief Threaded Zappy GUI client.
 *
 * This class owns the TCP client on a worker thread. The main/Flecs thread
 * communicates with it by queueing outgoing commands and polling complete
 * incoming protocol lines. All public methods are safe to call from the main
 * thread; access to the shared queues and status is guarded by an internal mutex.
 * @ingroup gui_network
 */
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

    /** @brief Starts a worker thread that connects to the given server.
     *
     * Stops any previous worker, clears the queues, sets status to Connecting,
     * then spawns the worker that owns the socket. Returns immediately; the
     * connection result is reported asynchronously through getStatus()/pollError().
     * @param host Server hostname or IP to resolve and connect to.
     * @param port Server TCP port.
     */
    void start(std::string host, std::uint16_t port);

    /** @brief Stops the worker and marks the client disconnected.
     *
     * Requests the worker to stop and joins it (blocking until it exits), then
     * sets status to Disconnected. Safe to call when no worker is running.
     */
    void stop();

    /** @brief Queues a command for the network thread to send.
     *
     * Non-blocking; the command is newline-terminated and enqueued, then sent
     * by the worker on its next loop iteration. The outgoing queue is bounded,
     * so the oldest command is dropped if it is full.
     * @param command Protocol command to send (a trailing newline is added if missing).
     */
    void send(std::string command);

    /** @brief Pops one complete server line, if available.
     *
     * Non-blocking. Intended to be polled every frame by the ECS thread.
     * @param[out] out Receives the next complete protocol line on success.
     * @return true if a line was dequeued into @p out, false if none is available.
     */
    bool pollLine(std::string &out);

    /** @brief Pops one network/transport error message, if available.
     *
     * Non-blocking.
     * @param[out] out Receives the next error message on success.
     * @return true if an error was dequeued into @p out, false if none is available.
     */
    bool pollError(std::string &out);

    /** @brief Returns the last known connection status.
     * @return The current ClientStatus, read under the internal mutex.
     */
    [[nodiscard]] ClientStatus getStatus() const;

    /** @brief Returns the last known connection status message.
     * @return A copy of the display-ready status string, read under the internal mutex.
     */
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

    /// Maximum number of queued commands or errors.
    static constexpr std::size_t maxQueueSize = 4096;
    /// Protects queues and visible status fields.
    mutable std::mutex mutex;
    /// Worker thread that owns the socket.
    std::jthread thread;
    /// Complete protocol lines received from the server.
    UnboundedQueue<std::string> incoming;
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
