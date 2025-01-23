#ifndef BOOK_HPP
#define BOOK_HPP

struct IncComparator {
    bool operator()(const std::shared_ptr<SamePriceQueue>& a, 
                    const std::shared_ptr<SamePriceQueue>& b) const {
        return a->price < b->price;
    }
};

struct DecComparator {
    bool operator()(const std::shared_ptr<SamePriceQueue>& a, 
                    const std::shared_ptr<SamePriceQueue>& b) const {
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
    std::string to_string() const;
    Book() = default;
};

#endif // BOOK_HPP
