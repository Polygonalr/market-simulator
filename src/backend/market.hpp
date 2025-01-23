#ifndef MARKET_HPP
#define MARKET_HPP

class Market {
  public:
    Market();
    ~Market();
    void send_order(ClientCommand order);
    static Market &get_instance();
    Market(Market const &) = delete;
    void operator=(Market const &) = delete;

  private:
    std::shared_mutex read_write_lock;
    std::unordered_map<std::string, Book> order_books;
};

#endif // MARKET_HPP
