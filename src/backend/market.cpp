#include <string>
#include <unordered_map>
#include <deque>
#include <mutex>
#include <memory>
#include <vector>
#include <cstdint>
#include <set>
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
    if (this->order_books.find(command.instrument) == this->order_books.end()) {
        this->order_books[command.instrument] = Book();
    }
    this->order_books[command.instrument].send_order(command);
}
