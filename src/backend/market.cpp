#include <boost/asio.hpp>
#include <string>
#include <unordered_map>
#include <deque>
#include <mutex>
#include <memory>
#include <vector>
#include <cstdint>
#include <map>
#include <shared_mutex>
#include "spq.hpp"
#include "engine.hpp"
#include "book.hpp"
#include "market.hpp"

Market::Market() {}

Market::~Market() {}

Market &Market::get_instance() {
    static Market instance;
    return instance;
}

void Market::send_order(ClientCommand command) {
    std::shared_lock lock(this->read_write_lock);
    if (this->order_books.find(command.instrument) == this->order_books.end()) {
        // instrument not found, get writer's lock and add it in.
        lock.unlock();
        std::unique_lock ulock(this->read_write_lock);
        // will do nothing if another thread has already added the instrument, so no further check is required.
        order_books.try_emplace(command.instrument);
    }
    // do not need to re-lock if lock is already unlocked in the if statemnet,
    // as the instrument is guaranteed to already exist at this point
    this->order_books[command.instrument].send_order(command);
}
