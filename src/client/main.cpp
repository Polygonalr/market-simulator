#include <boost/asio.hpp>
#include <iostream>
#include <random>
#include <thread>
#include <vector>
#include "util.hpp"

using namespace std;
using boost::asio::ip::tcp;

class Client {
  private:
    tcp::socket socket_;
    tcp::resolver::results_type endpoint_;
    DPRNG rng;
    vector<string> commands;

    void start() {
        for (auto command : commands) {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(rng.generate(0, 1000)));
            boost::asio::async_write(
                socket_, boost::asio::buffer(command),
                [this](boost::system::error_code ec, std::size_t) {
                    if (!ec) {
                        std::cout << "Sent command" << std::endl;
                    } else {
                        std::cout << "Error sending command: " << ec.message()
                                  << std::endl;
                    }
                });
        }
    }

  public:
    Client(boost::asio::io_context &io_context, const std::string &host,
           const std::string &port, unsigned int seed, vector<string> commands)
        : socket_(io_context), rng(seed), commands(commands) {
        tcp::resolver resolver(io_context);
        endpoint_ = resolver.resolve(host, port);
    }

    float gen_float() { return rng.generate(0.0f, 1.0f); }

    void connect() {
        boost::asio::async_connect(
            socket_, endpoint_,
            [this](boost::system::error_code ec, tcp::endpoint) {
                if (!ec) {
                    std::cout << "Connected successfully!" << std::endl;
                    start();
                } else {
                    std::cout << "Connection failed: " << ec.message()
                              << std::endl;
                }
            });
    }
};

int main(int argc, char *argv[]) {
    int number_of_clients;
    int seed;
    boost::asio::io_context io_context;
    vector<Client> clients;

    if (argc < 3) {
        fprintf(stderr,
                "Usage: %s <ip address> <port number> [seed] < <input>\n",
                argv[0]);
        return 1;
    }

    if (argc > 3) {
        seed = atoi(argv[3]);
    }

    cin >> number_of_clients;
    cout << "reading for " << number_of_clients << " client(s)\n";
    clients.reserve(number_of_clients);

    for (int i = 0; i < number_of_clients; i++) {
        std::string line;
        vector<string> commands;
        bool done = false;
        while (std::getline(std::cin, line)) {
            if (line == "")
                continue;
            if (line == "=") {
                done = true;
                break;
            }
            commands.push_back(line); // TODO validation
            // cout << line << endl;
        }
        if (!done) {
            cout << "Expected more input! Exiting early.\n";
            exit(1);
        }
        Client client{io_context, "localhost", "8080",
                      static_cast<unsigned int>(123 + i * 5),
                      std::move(commands)};
        clients.push_back(std::move(client));
    }
    for (auto &client : clients) {
        client.connect();
    }

    // create thread pool to deal with the io_contexts
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&io_context]() { io_context.run(); });
    }

    for (auto &thread : threads) {
        thread.join();
    }

    cout << "done!" << endl;
}