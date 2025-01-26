#ifndef ENGINE_HPP
#define ENGINE_HPP

class Client {
  private:
    uint32_t id;
    std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;

  public:
    Client(uint32_t id,
           std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr)
        : id(id), socket_ptr(socket_ptr) {}
    void do_read();
    void do_write(std::shared_ptr<std::string>);

    // Delete copy constructor and assignment
    Client(const Client &) = delete;
    Client &operator=(const Client &) = delete;

    // Allow moving if needed
    Client(Client &&) = default;
    Client &operator=(Client &&) = default;
};

class Engine {
  private:
    int next_client_id;
    std::vector<std::unique_ptr<Client>> client_data;
    std::mutex client_data_lock;

  public:
    Engine();
    ~Engine();
    static Engine &get_instance();
    void start_client_session(std::shared_ptr<boost::asio::ip::tcp::socket>);
};

enum CommandType { input_buy = 'B', input_sell = 'S', input_cancel = 'C' };

struct ClientCommand {
    CommandType type;
    // uint32_t order_id;
    uint32_t price;
    uint32_t count;
    std::string instrument;

    static ClientCommand parse_command(const std::string &);
    std::string to_string() const;
    friend std::ostream &operator<<(std::ostream &os, const ClientCommand &cc);
    
    operator Order();
};

#endif // ENGINE_HPP
