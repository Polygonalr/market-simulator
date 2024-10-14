struct Order {
    uint32_t order_id;
    uint32_t price;
    uint32_t count;
};

struct SamePriceQueue {
    std::mutex lock;
    std::deque<Order> orders;
    uint32_t price;
    uint32_t volume;

    bool can_match(Order);
    void add_order(Order);
    Order match_order(Order, intmax_t);
    void clear_queue();
};

class Book {
    std::shared_ptr<std::vector<std::shared_ptr<SamePriceQueue>>> buys;
    std::shared_ptr<std::vector<std::shared_ptr<SamePriceQueue>>> sells;

    std::shared_ptr<std::mutex> lock;
};
