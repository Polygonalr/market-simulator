#include <boost/asio.hpp>
#include <string>
#include <deque>
#include <mutex>
#include <iostream>
#include <shared_mutex>
#include "spq.hpp"

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
 * 
 * Assumed the SamePriceQueue is already locked by the current thread.
 */
Order SamePriceQueue::match_order(Order order) {
    while (this->orders.size() != 0 && order.count != 0) {
        Order next_order = this->orders.front();
        if (next_order.count > order.count) {
            // Fulfill the order in the orderbook partially
            std::cout << "Matched order " << order.order_id << " with "
                      << next_order.order_id << " at price " << next_order.price
                      << ", counts fulfilled: " << order.count << std::endl;
            this->orders.front().count -= order.count;
            this->volume -= order.count;
            order.count = 0;
            // return empty order since count = 0
            return order;
        }
        // Fulfill the order in the orderbook fully
        std::cout << "Matched order " << order.order_id << " with "
                  << next_order.order_id << " at price " << next_order.price
                  << ", counts fulfilled: " << next_order.count << std::endl;
        order.count -= next_order.count;
        this->volume -= next_order.count;
        this->orders.pop_front();
    }

    // No more orders to match, return the order with leftover count
    // This will be processed outside of this function to be added to respective SPQ
    return order;
}
