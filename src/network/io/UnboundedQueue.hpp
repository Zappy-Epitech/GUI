#pragma once

#include <deque>
#include <utility>

namespace net {

/// FIFO queue that preserves every value until it is consumed.
template <typename T>
class UnboundedQueue {
  public:
    /// Pushes a value without dropping older entries.
    void push(T value) {
        values.push_back(std::move(value));
    }

    /// Pops the oldest value into out.
    bool pop(T &out) {
        if (values.empty()) {
            return false;
        }
        out = std::move(values.front());
        values.pop_front();
        return true;
    }

    /// Removes all queued values.
    void clear() {
        values.clear();
    }

  private:
    /// Stored FIFO values.
    std::deque<T> values;
};

} // namespace net
