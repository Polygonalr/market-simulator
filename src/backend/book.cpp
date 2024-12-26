#include <boost/asio.hpp>
#include <cstdint>
#include <mutex>
#include <memory>
#include <vector>
#include <set>
#include <deque>
#include <iostream>
#include "engine.hpp"
#include "book.hpp"

Book::Book()
    : buys(std::set<SamePriceQueue, decltype(spq_inc_cmp)>()),
      sells(std::set<SamePriceQueue, decltype(spq_dec_cmp)>()) {};

void Book::send_order(ClientCommand command) {
    std::lock_guard<std::mutex> book_lock {this->lock};
    switch(command.type) {
        case input_buy:
            std::cout << "Received buy order\n";
            break;
        case input_sell:
            std::cout << "Received sell order\n";
            break;
        default:
            std::cout << "Received cancel order\n";
            break;
    }
}
