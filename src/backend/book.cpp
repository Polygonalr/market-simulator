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

bool SamePriceQueue::can_match(Order order) {
    if (order.order_type == buy) {
        return price <= order.price;
    }
    return order.price <= price;
}

void SamePriceQueue::add_order(Order order) {
    this->orders.push_back(order);
    this->volume += order.count;
}

/**
 * Takes in the order to be matched with the orders from current SPQ, prints all
 * the matched orders and returns the order with its new volume after orders are
 * matched.
 */
Order SamePriceQueue::match_order(Order order) {
    while (this->orders.size() != 0 && order.count != 0) {
        Order next_order = this->orders.front();
        if (next_order.count > order.count) {
            // Fulfill the order in the orderbook partially
            this->orders.front().count -= order.count;
            this->volume -= order.count;
            order.count = 0;

            std::cout << "Matched order " << order.order_id << " with "
                      << next_order.order_id << " at price " << next_order.price
                      << ", counts fulfilled: " << order.count << std::endl;

            return order;
        }
        // Fulfill the order in the orderbook fully
        order.count -= next_order.count;
        this->volume -= next_order.count;
        this->orders.pop_front();
        std::cout << "Matched order " << order.order_id << " with "
                  << next_order.order_id << " at price " << next_order.price
                  << ", counts fulfilled: " << next_order.count << std::endl;
    }
    return order;
}

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
