#include <string>
#include <netinet/in.h>

class ClientConnection {
    public:
        ClientConnection(int);
        ~ClientConnection() { this->close_connection(); };
        void send_message(std::string message);
        std::string receive_message();
        void close_connection();
    private:
        int connfd;
};

class TCPServer {
    public:
        static const int DEFAULT_PORT;
        TCPServer(int);
        TCPServer() : TCPServer(0) {}
        ~TCPServer();
        ClientConnection wait_and_accept_connection();
    private:
        int server_socket;
};
