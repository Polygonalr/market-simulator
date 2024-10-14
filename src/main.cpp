#include <iostream>
#include <memory>
#include <boost/asio.hpp>

#include "io.hpp"
#include "engine.hpp"

using namespace std;

// TODO signal handling for graceful clean up

int main() {
  boost::asio::io_context io_context;
  short port = 8080;
  TCPServer server(io_context, port);
  cout << "Server starting~!\n";
  Engine::get_instance(); // pre-initialize
  io_context.run();
}