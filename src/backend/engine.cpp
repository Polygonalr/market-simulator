#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <sstream>
#include "engine.hpp"

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

void Engine::client_do_read(
    shared_ptr<boost::asio::ip::tcp::socket> socket_ptr) {
    int max_length = 128;
    auto read_buffer = make_shared<string>(max_length, '\0');
    socket_ptr->async_read_some(
        boost::asio::buffer(*read_buffer, max_length),
        [this, socket_ptr, read_buffer](boost::system::error_code ec,
                                        std::size_t) {
            if (ec == boost::asio::error::eof ||
                ec == boost::asio::error::connection_reset) {
                cout << "Client "
                     << " disconnected\n";
            } else if (!ec) {
                cout << "Received message: " << *read_buffer << endl;
                client_do_write(socket_ptr, *read_buffer);
            }
        });
}

void Engine::client_do_write(shared_ptr<boost::asio::ip::tcp::socket> socket,
                             string write_buffer) {
    socket->async_write_some(
        boost::asio::buffer(write_buffer),
        [this, socket](boost::system::error_code ec, std::size_t) {
            if (!ec) {
                cout << "Sent message back" << endl;
                client_do_read(socket);
            }
        });
}

Engine::Engine() { cout << "Engine created" << endl; }

Engine::~Engine() { cout << "Engine destroyed" << endl; }

Engine &Engine::get_instance() {
    static Engine instance;
    return instance;
}

void Engine::start_client_session(
    shared_ptr<boost::asio::ip::tcp::socket> socket) {
    int client_id = next_client_id++;
    cout << "Client " << client_id << " connected" << endl;
    client_do_read(socket);
}

ClientCommand ClientCommand::parse_command(string s) {
    vector<string> splitted_string = split(s, ' ');
    string instrument = splitted_string[4];
    if (instrument.size() > 8) {
        instrument = instrument.substr(0, 8);
    }
    std::transform(instrument.begin(), instrument.end(), instrument.begin(),
                   ::toupper);
    ClientCommand cc = ClientCommand{
        .type = static_cast<CommandType>(splitted_string[0][0]),
        .order_id = static_cast<uint32_t>(stoul(splitted_string[1])),
        .price = static_cast<uint32_t>(stoul(splitted_string[2])),
        .count = static_cast<uint32_t>(stoul(splitted_string[3])),
    };
    strcpy(cc.instrument, instrument.c_str());
    return cc;
}

string ClientCommand::to_string() const {
    stringstream ss;
    ss << static_cast<char>(type) << " " << order_id << " " << price << " "
       << count << " " << instrument;
    return ss.str();
}

std::ostream &operator<<(std::ostream &os, const ClientCommand &cc) {
    os << cc.to_string();
    return os;
}
