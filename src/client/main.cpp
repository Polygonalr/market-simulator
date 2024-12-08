#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <iostream>
#include <random>
#include <thread>
#include <vector>
#include "util.hpp"

using namespace std;
using boost::asio::ip::tcp;
using boost::asio::steady_timer;

#define DEFAULT_RNG_SEED 123
#define NUM_THREADS 2

class Client {
  private:
    tcp::socket socket_;
    steady_timer timer_;
    tcp::resolver::results_type endpoint_;
    DPRNG rng;
    vector<string> commands;
    int curr_command_idx;

    void start() {
        send_next_command();
    }

    void send_next_command() {
        if (this->curr_command_idx != this->commands.size()) {
            timer_.expires_after(std::chrono::milliseconds(rng.generate(0, 1000)));
            timer_.async_wait([this](const boost::system::error_code &ec) {
                if (!ec) {
                    boost::asio::async_write(
                    socket_, boost::asio::buffer(this->commands[this->curr_command_idx]),
                    [this](boost::system::error_code ec, std::size_t) {
                        if (!ec) {
                            std::cout << "Sent command" << std::endl;
                        } else {
                            std::cout << "Error sending command: " << ec.message()
                                    << std::endl;
                        }
                    });
                    this->curr_command_idx++;
                    this->send_next_command();
                } else {
                    std::cout << "Timer error: " << ec.message() << std::endl;
                }
            });
        }
    }

  public:
    Client(boost::asio::io_context &io_context, const std::string &host,
           const std::string &port, unsigned int seed, vector<string> commands)
        : socket_(io_context), timer_(io_context), rng(seed), commands(commands), curr_command_idx(0) {
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
                    this->start();
                } else {
                    std::cout << "Connection failed: " << ec.message()
                              << std::endl;
                }
            });
    }
};

int main(int argc, char *argv[]) {
    int number_of_clients;
    int seed = DEFAULT_RNG_SEED;
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
        Client client{io_context, "localhost", argv[2],
                      static_cast<unsigned int>(seed + i * 4),
                      std::move(commands)};
        clients.push_back(std::move(client));
    }
    for (auto &client : clients) {
        client.connect();
    }

    // create thread pool to deal with the io_contexts
    std::vector<std::thread> threads;
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&io_context]() { io_context.run(); });
    }

    for (auto &thread : threads) {
        thread.join();
    }

    cout << "done!" << endl;
}