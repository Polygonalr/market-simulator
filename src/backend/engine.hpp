class Engine {
  private:
    std::atomic<int> next_client_id;
    void client_do_read(std::shared_ptr<boost::asio::ip::tcp::socket>);
    void client_do_write(std::shared_ptr<boost::asio::ip::tcp::socket>,
                         std::string);

  public:
    Engine();
    ~Engine();
    static Engine &get_instance();
    void start_client_session(std::shared_ptr<boost::asio::ip::tcp::socket>);
};

enum CommandType { input_buy = 'B', input_sell = 'S', input_cancel = 'C' };

struct ClientCommand {
    CommandType type;
    uint32_t order_id;
    uint32_t price;
    uint32_t count;
    char instrument[9];

    static ClientCommand parse_command(std::string);
    std::string to_string() const;
    friend std::ostream &operator<<(std::ostream &os, const ClientCommand &cc);
};