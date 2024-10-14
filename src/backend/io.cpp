#include <boost/asio.hpp>
#include <iostream>
#include <memory>

#include "engine.hpp"
#include "io.hpp"

using boost::asio::ip::tcp;

void TCPServer::do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                Engine::get_instance().start_client_session(
                    std::make_shared<tcp::socket>(std::move(socket)));
            }
            do_accept(); // continue accepting connections
        });
}