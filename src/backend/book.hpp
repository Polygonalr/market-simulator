#ifndef BOOK_HPP
#define BOOK_HPP

struct IncComparator {
    bool operator()(uint32_t a, uint32_t b) const { return a < b; }
};

struct DecComparator {
    bool operator()(uint32_t a, uint32_t b) const { return a > b; }
};

class Book {
  private:
    std::map<uint32_t, std::shared_ptr<SamePriceQueue>, IncComparator> buys;
    std::map<uint32_t, std::shared_ptr<SamePriceQueue>, DecComparator> sells;
    std::mutex lock;

  public:
    void create_spq(uint32_t price, OrderType type);
    void add_order_to_spq(Order order);
    void send_order(ClientCommand command);
    void get_queue_locks(
        Order order, std::deque<std::unique_lock<std::mutex>> &spq_locks,
        std::deque<std::shared_ptr<SamePriceQueue>> &locked_spqs,
        std::shared_ptr<SamePriceQueue> &spq_to_rest_at,
        std::unique_lock<std::mutex> &resting_spq_lock);
    std::string to_string() const;
    Book() = default;
};

#endif // BOOK_HPP
