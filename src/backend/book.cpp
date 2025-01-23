#include <boost/asio.hpp>
#include <cstdint>
#include <mutex>
#include <memory>
#include <vector>
#include <set>
#include <deque>
#include <iostream>
#include "engine.hpp"
#include "spq.hpp"
#include "book.hpp"

void Book::create_spq(uint32_t price, OrderType type) {
    switch (type) {
    case buy:
        this->buys.insert(std::make_shared<SamePriceQueue>(price));
        break;
    case sell:
        this->sells.insert(std::make_shared<SamePriceQueue>(price));
        break;
    }
}

void Book::add_order_to_spq(Order order) {
    std::lock_guard<std::mutex> book_lock{this->lock};
    if (order.order_type == buy) {
        for (auto spq : this->sells) {
            if (spq->can_match(order)) {
                order = spq->match_order(order);
            }
        }
        if (order.count != 0) {
            for (auto spq : this->buys) {
                if (spq->price == order.price) {
                    spq->add_order(order);
                    return;
                }
            }
            this->create_spq(order.price, buy);
            this->buys.begin()->get()->add_order(order);
        }
    } else {
        for (auto spq : this->buys) {
            if (spq->can_match(order)) {
                order = spq->match_order(order);
            }
        }
        if (order.count != 0) {
            for (auto spq : this->sells) {
                if (spq->price == order.price) {
                    spq->add_order(order);
                    return;
                }
            }
            this->create_spq(order.price, sell);
            this->sells.begin()->get()->add_order(order);
        }
    }

}

void Book::send_order(ClientCommand command) {
    std::lock_guard<std::mutex> book_lock{this->lock};
    switch (command.type) {
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

/**
 * Helper function to give a brief overview of the current Book object
 */
std::string Book::to_string() const {
    std::string result;
    result += "Buys:\n";
    for (auto spq : this->buys) {
        result += std::to_string(spq->price) + " " + std::to_string(spq->volume) + "\n";
    }
    result += "Sells:\n";
    for (auto spq : this->sells) {
        result += std::to_string(spq->price) + " " + std::to_string(spq->volume) + "\n";
    }
    return result;
}
