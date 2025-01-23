#ifndef SPQ_HPP
#define SPQ_HPP

enum OrderType { buy = 'B', sell = 'S' };

struct Order {
    OrderType order_type;
    uint32_t order_id;
    uint32_t price;
    uint32_t count;
};

struct SamePriceQueue {
    mutable std::mutex lock;
    std::deque<Order> orders;
    uint32_t price;
    uint32_t volume;

    bool can_match(Order);
    void add_order(Order);
    Order match_order(Order);
    void clear_queue();
    SamePriceQueue(int price): price(price) {};
};

#endif // SPQ_HPP
