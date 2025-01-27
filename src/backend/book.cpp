#include <boost/asio.hpp>
#include <cstdint>
#include <mutex>
#include <memory>
#include <vector>
#include <map>
#include <deque>
#include <iostream>
#include "spq.hpp"
#include "engine.hpp"
#include "book.hpp"

void Book::create_spq(uint32_t price, OrderType type) {
    switch (type) {
    case buy:
        this->buys[price] = std::make_shared<SamePriceQueue>(price);
        break;
    case sell:
        this->sells[price] = std::make_shared<SamePriceQueue>(price);
        break;
    }
}

void Book::send_order(ClientCommand command) {
    std::deque<std::unique_lock<std::mutex>> spq_locks;
    std::deque<std::shared_ptr<SamePriceQueue>> locked_spqs; // for matching
    std::shared_ptr<SamePriceQueue>
        spq_to_rest_at = nullptr; // for resting the remaining volume
    std::unique_lock<std::mutex> resting_spq_lock;

    Order order = static_cast<Order>(command);

    {
        std::lock_guard<std::mutex> book_lock{this->lock};
        this->get_queue_locks(order, spq_locks, locked_spqs, spq_to_rest_at,
                              resting_spq_lock);
    }

    while (!locked_spqs.empty()) {
        order = locked_spqs[0]->match_order(order);
        spq_locks.pop_front(); // should automatically give up mutex, TODO check
                               // this again
        locked_spqs.pop_front();
        // TODO handle removal of empty SPQs
    }

    if (spq_to_rest_at != nullptr) {
        spq_to_rest_at->add_order(order);
    }
}

void Book::get_queue_locks(
    Order order, std::deque<std::unique_lock<std::mutex>> &spq_locks,
    std::deque<std::shared_ptr<SamePriceQueue>> &locked_spqs,
    std::shared_ptr<SamePriceQueue> &spq_to_rest_at,
    std::unique_lock<std::mutex> &resting_spq_lock) {
    int remaining_volume = order.count;

    switch (order.order_type) {
    case buy:
        for (auto it : this->sells) {
            auto &key = it.first;
            auto &val = it.second;
            if (val->volume == 0) {
                continue;
            }
            // need to lock the queue first before can_match so that no other
            // threads can touch the queue after can_match
            auto current_spq_lock = std::unique_lock(val->lock);

            if (val->can_match(order)) {
                spq_locks.push_back(std::move(current_spq_lock));
                locked_spqs.push_back(val);
                remaining_volume -= val->volume;
                if (remaining_volume <= 0) {
                    break;
                }
            } else {
                break;
            }
        }
        break;
    case sell:
        for (auto it : this->buys) {
            auto &key = it.first;
            auto &val = it.second;
            if (val->volume == 0) {
                continue;
            }
            // need to lock the queue first before can_match so that no other
            // threads can touch the queue after can_match
            auto current_spq_lock = std::unique_lock(val->lock);

            if (val->can_match(order)) {
                spq_locks.push_back(std::move(current_spq_lock));
                locked_spqs.push_back(val);
                remaining_volume -= val->volume;
                if (remaining_volume <= 0) {
                    break;
                }
            } else {
                break;
            }
        }
        break;
    }

    if (remaining_volume > 0) {
        // also obtain the lock to the SPQ to rest the order at if there are
        // remaining volume
        bool found = false;
        switch (order.order_type) {
        case buy:
            if (auto search = this->buys.find(order.price);
                search != this->buys.end()) {
                resting_spq_lock = std::unique_lock(search->second->lock);
                spq_to_rest_at = search->second;
                found = true;
            }
            break;
        case sell:
            if (auto search = this->sells.find(order.price);
                search != this->sells.end()) {
                resting_spq_lock = std::unique_lock(search->second->lock);
                spq_to_rest_at = search->second;
                found = true;
            }
            break;
        }

        if (!found) {
            std::shared_ptr<SamePriceQueue> new_spq =
                std::make_shared<SamePriceQueue>(order.price);

            switch (order.order_type) {
            case buy:
                this->buys[order.price] = new_spq;
                break;
            case sell:
                this->sells[order.price] = new_spq;
                break;
            }
            resting_spq_lock = std::unique_lock(new_spq->lock);
            spq_to_rest_at = new_spq;
        }
    }
}

/**
 * Helper function to give a brief overview of the current Book object
 */
std::string Book::to_string() const {
    std::string result;
    result += "Buys:\n";
    for (auto it : this->buys) {
        auto &spq = it.second;
        result += std::to_string(spq->price) + " " +
                  std::to_string(spq->volume) + "\n";
    }
    result += "Sells:\n";
    for (auto it : this->sells) {
        auto &spq = it.second;
        result += std::to_string(spq->price) + " " +
                  std::to_string(spq->volume) + "\n";
    }
    return result;
}
