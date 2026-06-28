/**
 * @file BoundedQueue.hpp
 * @ingroup gui_network
 * @brief Fixed-capacity FIFO queue that drops the oldest value when full.
 */
#pragma once

#include <cstddef>
#include <deque>
#include <utility>

namespace net {

/** @brief FIFO queue with a fixed maximum size. When full, oldest values are dropped.
 * @ingroup gui_network
 */
template <typename T, std::size_t MaxSize>
class BoundedQueue {
  public:
    /// Pushes a value, dropping the oldest one if full.
    void push(T value) {
        if (values.size() >= MaxSize) {
            values.pop_front();
        }
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
