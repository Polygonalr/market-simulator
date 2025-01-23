#ifndef BOOK_HPP
#define BOOK_HPP

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

struct IncComparator {
    bool operator()(const std::shared_ptr<SamePriceQueue>& a, 
                    const std::shared_ptr<SamePriceQueue>& b) const {
        // Compare shared_ptrs based on their contained values
        return a->price < b->price;
    }
};

struct DecComparator {
    bool operator()(const std::shared_ptr<SamePriceQueue>& a, 
                    const std::shared_ptr<SamePriceQueue>& b) const {
        // Compare shared_ptrs based on their contained values
        return a->price > b->price;
    }
};

class Book {
  private:
    std::set<std::shared_ptr<SamePriceQueue>, IncComparator> buys;
    std::set<std::shared_ptr<SamePriceQueue>, DecComparator> sells;
    std::mutex lock;
  public:
    void create_spq(uint32_t price, OrderType type);
    void add_order_to_spq(Order order);
    void send_order(ClientCommand command);
    Book() = default;
};

#endif // BOOK_HPP
