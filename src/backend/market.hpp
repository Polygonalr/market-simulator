class Market {
  public:
    Market();
    ~Market();
    void send_order(ClientCommand order);
    static Market &get_instance();
    Market(Market const &) = delete;
    void operator=(Market const &) = delete;

  private:
    std::unordered_map<std::string, Book> order_books;
};