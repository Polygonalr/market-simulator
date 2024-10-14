using boost::asio::ip::tcp;

class TCPServer {
  private:
    tcp::acceptor acceptor_;
    void do_accept();

  public:
    TCPServer(boost::asio::io_context &io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        do_accept();
    }
};
