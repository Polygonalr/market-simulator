#ifndef BOOK_HPP
#define BOOK_HPP

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

auto spq_inc_cmp = [](SamePriceQueue a, SamePriceQueue b) { return a.price < b.price; };
auto spq_dec_cmp = [](SamePriceQueue a, SamePriceQueue b) { return a.price > b.price; };

class Book {
  private:
    std::set<SamePriceQueue, decltype(spq_inc_cmp)> buys;
    std::set<SamePriceQueue, decltype(spq_dec_cmp)> sells;
    std::mutex lock;
  public:
    Book();
    void send_order(ClientCommand command);
};

#endif // BOOK_HPP
