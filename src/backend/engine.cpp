#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <sstream>
#include <shared_mutex>
#include <deque>
#include <set>
#include "engine.hpp"
#include "book.hpp"
#include "market.hpp"

using namespace std;
using boost::asio::ip::tcp;

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;

    while (getline(ss, item, delim)) {
        result.push_back(item);
    }

    return result;
}

void Client::do_read() {
    int max_length = 128;
    auto read_buffer = make_shared<string>(max_length, '\0');
    socket_ptr->async_read_some(
        boost::asio::buffer(*read_buffer, max_length),
        [this, read_buffer](boost::system::error_code ec, std::size_t) {
            if (ec == boost::asio::error::eof ||
                ec == boost::asio::error::connection_reset) {
                cout << "Client disconnected\n";
            } else if (!ec) {
                cout << "Received message: " << *read_buffer << endl;
                this->do_write(read_buffer);
                ClientCommand command =
                    ClientCommand::parse_command(string(*read_buffer));
                Market& market_instance = Market::get_instance();
                market_instance.send_order(command);
            }
        });
}

void Client::do_write(std::shared_ptr<std::string> write_buffer) {
    socket_ptr->async_write_some(
        boost::asio::buffer(*write_buffer),
        [this](boost::system::error_code ec, std::size_t) {
            if (!ec) {
                cout << "Sent message back" << endl;
                this->do_read();
            }
        });
}

Engine::Engine() : next_client_id(0) { cout << "Engine created" << endl; }

Engine::~Engine() { cout << "Engine destroyed" << endl; }

Engine &Engine::get_instance() {
    static Engine instance;
    return instance;
}

void Engine::start_client_session(
    shared_ptr<boost::asio::ip::tcp::socket> socket) {
    int client_id;
    {
        // critical section
        std::lock_guard<std::mutex> guard(client_data_lock);
        client_id = next_client_id++;
        client_data.emplace_back(
            std::make_unique<Client>(Client{(uint32_t)client_id, socket}));
    }
    cout << "Client " << client_id << " connected" << endl;
    client_data[client_id]->do_read();
}

/**
 * Parses a string and returns a ClientCommand object
 * TODO For now there will be index-out-of-bounds error if string is not of the correct format lmao
 * 
 * @param s the input string
 * @return resultant ClientCommand object
 */
ClientCommand ClientCommand::parse_command(const string &s) {
    std::vector<string> splitted_string = split(s, ' ');
    string instrument = splitted_string[1];
    // limit instrument string to 8 characters
    if (instrument.size() > 8) {
        instrument = instrument.substr(0, 8);
    }
    std::transform(instrument.begin(), instrument.end(), instrument.begin(),
                   ::toupper);
    ClientCommand cc = ClientCommand{
        .type = static_cast<CommandType>(splitted_string[0][0]),
        .price = static_cast<uint32_t>(stoul(splitted_string[3])),
        .count = static_cast<uint32_t>(stoul(splitted_string[2])),
    };
    cc.instrument = instrument;
    return cc;
}

/**
 * Convert ClientCommand object to a string
 * 
 * @param self
 * @return the ClientCommand object as a string
 */
string ClientCommand::to_string() const {
    stringstream ss;
    ss << static_cast<char>(type) << " " << price << " " << count << " "
       << instrument;
    return ss.str();
}

std::ostream &operator<<(std::ostream &os, const ClientCommand &cc) {
    os << cc.to_string();
    return os;
}
